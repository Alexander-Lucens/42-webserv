#pragma once

#include "Connection.hpp"
#include <cctype>
#include <string>
#include <algorithm>

Connection::Connection() {}
Connection::~Connection() {}

const std::string& Connection::get_read_buffer() const {
    return (this->read_buffer);
}

void Connection::on_readable() {
    int fd = this->socket.getFd();
    ssize_t bytes_read;
    char buffer[1024];

    while ((bytes_read = ::read(fd, buffer, sizeof(buffer))) > 0) {
        this->read_buffer.append(buffer, bytes_read);
	}
    if (bytes_read == 0) {
        scan_buffer();
        return;
    }
    // assuming bytes_read == -1
    if (errno != EAGAIN && errno != EWOULDBLOCK) { // real error
        // code block
        return;
    }
    return; // no data currently left to read, come back later
}


// void Connection::on_writable() {}

// void Connection::on_error() {}


bool Connection::parse_request_line() {
    std::string::size_type start_pos = 0;
    std::string::size_type end_pos;

    // building string encompassing only request line to work on
    end_pos = this->read_buffer.find("\r\n");
    if (end_pos == std::string::npos)
        throw std::runtime_error("Error: Incomplete request line");
    std::string request_line = this->read_buffer.substr(start_pos, end_pos - start_pos);

    // building method
    start_pos = end_pos + 1;
    end_pos = request_line.find(" ", start_pos);
    if (end_pos == std::string::npos)
        throw std::runtime_error("Error: Faulty request line");
    this->request.method = request_line.substr(start_pos, end_pos - start_pos);

    // building uri
    start_pos = end_pos + 1;
    end_pos = request_line.find(" ", start_pos);
    if (end_pos == std::string::npos)
        throw std::runtime_error("Error: Faulty request line");
    this->request.uri = request_line.substr(start_pos, end_pos - start_pos);

    // building version
    start_pos = end_pos + 1;
    end_pos = request_line.find(" ", start_pos);
    if (end_pos == std::string::npos)
        throw std::runtime_error("Error: Faulty request line");
    this->request.version = request_line.substr(start_pos, end_pos - start_pos);

    return (true);
}

bool Connection::parse_request_headers() {
    std::string::size_type start_pos;
    std::string::size_type end_pos;

    // building string encompassing only headers block to work on
    start_pos = (this->read_buffer.find("\r\n")) + 2;
    end_pos = this->read_buffer.find("\r\n\r\n");
    if (end_pos == std::string::npos)
        throw std::runtime_error("Error: Incomplete headers block");
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
            throw std::runtime_error("Error: Faulty header");
        
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

    return (true);
}

bool Connection::parse_request_body() {
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
        throw std::runtime_error("Error: Incomplete body");

    return (true);
}


void Connection::scan_buffer() {
    switch(this->request.state) {
        case Request::REQUEST_LINE: {
            if (parse_request_line())
                this->request.state = Request::REQUEST_HEADERS;
            else
                break;
        }
        case Request::REQUEST_HEADERS: {
            if (parse_request_headers())
                this->request.state = Request::REQUEST_BODY;
            else
                break;
        }
        case Request::REQUEST_BODY: {
            if (this->request.headers.count("content-length")) {
                if (parse_request_body())
                    this->request.state = Request::DONE;
                else
                    break;
            }
            else
                this->request.state = Request::DONE;
        }
        case Request::DONE: {
            
            break;
        }
        case Request::ERROR: {
            // code block
            break;
        }
    }
}
