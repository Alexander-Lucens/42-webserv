#pragma once

#include "Connection.hpp"
#include <cctype>
#include <string>
#include <algorithm>

enum ParseResult {PARSE_OK, PARSE_INCOMPLETE, PARSE_ERROR};
enum ScanResult {CONTINUE, STOP};

Connection::Connection() {}

Connection::Connection(int fd): socket(fd) {}

Connection::~Connection() {}

const std::string& Connection::get_read_buffer() const {
    return (this->read_buffer);
}

void Connection::on_readable() {
    int     fd = this->socket.getFd();
    ssize_t bytes_read;
    char    buffer[1024];
    int     result;

    while ((bytes_read = ::read(fd, buffer, sizeof(buffer))) > 0) {
        this->read_buffer.append(buffer, bytes_read);
	}

    if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
        send_error_response();
        return;
    }

    while (true) {
        result = scan_buffer();
        if (result == STOP) 
            return;
    }
}

int Connection::parse_request_line() {
    std::string::size_type start_pos = 0;
    std::string::size_type end_pos;

    // building string encompassing only request line to work on
    end_pos = this->read_buffer.find("\r\n");
    if (end_pos == std::string::npos)
        return (PARSE_INCOMPLETE);
    std::string request_line = this->read_buffer.substr(start_pos, end_pos - start_pos);

    // building method
    start_pos = end_pos + 1;
    end_pos = request_line.find(" ", start_pos);
    if (end_pos == std::string::npos)
        return (PARSE_ERROR);
    this->request.method = request_line.substr(start_pos, end_pos - start_pos);

    // building uri
    start_pos = end_pos + 1;
    end_pos = request_line.find(" ", start_pos);
    if (end_pos == std::string::npos)
        return (PARSE_ERROR);
    this->request.uri = request_line.substr(start_pos, end_pos - start_pos);

    // building version
    start_pos = end_pos + 1;
    end_pos = request_line.find(" ", start_pos);
    if (end_pos == std::string::npos)
        return (PARSE_ERROR);
    this->request.version = request_line.substr(start_pos, end_pos - start_pos);

    return (PARSE_OK);
}

int Connection::parse_request_headers() {
    std::string::size_type start_pos;
    std::string::size_type end_pos;

    // building string encompassing only headers block to work on
    start_pos = (this->read_buffer.find("\r\n")) + 2;
    end_pos = this->read_buffer.find("\r\n\r\n");
    if (end_pos == std::string::npos)
        return (PARSE_INCOMPLETE);
    std::string headers_block = this->read_buffer.substr(start_pos, end_pos - start_pos);

    // turning the whole string lowercase
    std::transform(headers_block.begin(), headers_block.end(), headers_block.begin(), 
        static_cast<int(*)(int)>(std::tolower));

    start_pos = 0;
    std::string::size_type headers_block_size = headers_block.size();

    // loop to handle as many individual headers as necessary
    while (start_pos < headers_block_size) {
        end_pos = headers_block.find(":", start_pos);
        if (end_pos == std::string::npos)
            return (PARSE_ERROR);
        
        std::string key = headers_block.substr(start_pos, end_pos - start_pos);

        start_pos = end_pos + 1;
        while (start_pos < headers_block_size &&
        std::isspace(static_cast<unsigned char>(headers_block[start_pos])))
            start_pos++;
        end_pos = headers_block.find("\r\n", start_pos);
        if (end_pos == std::string::npos)
            end_pos = headers_block_size;
            
        std::string value = headers_block.substr(start_pos, end_pos - start_pos);

        this->request.headers[key] = value;
        start_pos = end_pos + 2;
    }

    return (PARSE_OK);
}

int Connection::parse_request_body() {
    std::string::size_type start_pos;
    std::string::size_type end_pos;

    std::size_t content_length = static_cast<std::size_t>
        (std::strtoul(this->request.headers["content-length"].c_str(), NULL, 10));

    start_pos = this->read_buffer.find("\r\n\r\n");
    start_pos += 4;
    end_pos = read_buffer.size();
    if ((end_pos - start_pos) >= content_length)
        this->request.body = this->read_buffer.substr(start_pos, content_length);
    else
        return (PARSE_INCOMPLETE);

    return (PARSE_OK);
}

void Connection::send_error_response() {
    // send back error 400
}


int Connection::scan_buffer() {
    int result;
    switch(this->request.state) {
        case Request::REQUEST_LINE: {
            result = parse_request_line();
            if (result == PARSE_OK) {
                this->request.state = Request::REQUEST_HEADERS;
                return (CONTINUE);
            }
            else if (result == PARSE_INCOMPLETE)
                return (STOP);
            else if (result == PARSE_ERROR) {
                this->request.state = Request::ERROR;
                return (CONTINUE);
            }
        }
        case Request::REQUEST_HEADERS: {
            result = parse_request_headers();
            if (result == PARSE_OK) {
                this->request.state = Request::REQUEST_BODY;
                return (CONTINUE);
            }
            else if (result == PARSE_INCOMPLETE)
                return (STOP);
            else if (result == PARSE_ERROR) {
                this->request.state = Request::ERROR;
                return (CONTINUE);
            }
        }
        case Request::REQUEST_BODY: {
            if (this->request.headers.count("content-length")) {
                result = parse_request_body();
                if (result == PARSE_OK) {
                    this->request.state = Request::DONE;
                    return (CONTINUE);
                }
                else if (result == PARSE_INCOMPLETE)
                    return (STOP);
                else if (result == PARSE_ERROR) {
                    this->request.state = Request::ERROR;
                    return (CONTINUE);
                }
            }
            else {
                this->request.state = Request::DONE;
                return (CONTINUE);
            }
        }
        case Request::DONE: {
            this->request.execute();
            this->clean_buffer_for_new_request();
            this->request.clear();
            this->request.state = Request::REQUEST_LINE;
            return (CONTINUE);
        }
        case Request::ERROR: {
            send_error_response();
            this->clean_buffer_for_new_request();
            this->request.clear();
            this->request.state = Request::REQUEST_LINE;
            return (CONTINUE);
        }
    }
}

void Connection::clean_buffer_for_new_request() {
    std::string::size_type start_pos;

    start_pos = this->read_buffer.find("\r\n\r\n");
    if (start_pos == std::string::npos)
        return; // error in buffer
    start_pos += 4;

    if (this->request.headers.count("content-length"))
    {
        std::size_t content_length = static_cast<std::size_t>
            (std::strtoul(this->request.headers["content-length"].c_str(), NULL, 10));
        start_pos += content_length;

    }
    read_buffer.erase(0, start_pos);
}
