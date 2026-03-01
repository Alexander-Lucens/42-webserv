#include "Colors.hpp"
#include "Connection.hpp"
#include <cctype>
#include <string>
#include <algorithm>

enum ParseResult {PARSE_OK, PARSE_INCOMPLETE, PARSE_ERROR};
enum ScanResult {CONTINUE, STOP};

// Connection::Connection() {}

Connection::Connection(int fd) : _fd(fd) {
    this->request.state = Request::REQUEST_LINE;
}

// Connection::Connection(Socket &socket): socket(socket) {}

Connection::~Connection() {
    if (_fd != -1) {
        ::close(_fd);
    }
}

const std::string& Connection::get_read_buffer() const {
    return (this->read_buffer);
}

bool Connection::on_readable() {
    ssize_t bytes_read;
    char    buffer[1024];
    int     result;

    while ((bytes_read = ::read(_fd, buffer, sizeof(buffer))) > 0) {
        this->read_buffer.append(buffer, bytes_read);
	}

    if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
        this->request.state = Request::ERROR;
        scan_buffer(); // one last time to execute the bad request via an error 400 response and close the connection
        return false;
    }

    while (true) {
        result = scan_buffer();
        if (result == STOP) 
            return true;
    }
}

/**
 * @brief Method to send back all data from buffer to client
 * 
 * @return true 
 * @return false 
 */
bool Connection::on_writable() {
    if (this->write_buffer.empty()) {
        return true; 
    }
    ssize_t bytes_sent = write(_fd, this->write_buffer.c_str(), this->write_buffer.size());

    if (bytes_sent < 0) {
        return false; 
    }
    this->write_buffer.erase(0, bytes_sent);
    return true; 
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
	start_pos = 0;
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

	if (!Utils::is_valid_uri(request.uri)) {
		LOG_WARNING("Invalid URI: " << request.uri);
		return PARSE_ERROR;
	}
    // building version
	start_pos = end_pos + 1;
	this->request.version = request_line.substr(start_pos);

	// Optionally validate version
	if (this->request.version.empty() || 
		(this->request.version != "HTTP/1.1" && this->request.version != "HTTP/1.0"))
		return (PARSE_ERROR);
		
    parse_uri();

    return (PARSE_OK);
}

void Connection::parse_uri() {
    std::string::size_type start_pos = 0;
    std::string::size_type end_pos;

    start_pos = 0;
    end_pos = this->request.uri.find("?", start_pos);
    if (end_pos == std::string::npos)
    {
        this->request.path = this->request.uri.substr(start_pos, this->request.uri.size() - start_pos);
        return;
    }

    this->request.path = this->request.uri.substr(start_pos, end_pos - start_pos);
    
	start_pos = end_pos + 1;
    end_pos = this->request.uri.size();
    this->request.query_string = this->request.uri.substr(start_pos, end_pos - start_pos);
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

    start_pos = 0;
    std::string::size_type headers_block_size = headers_block.size();

    // loop to handle as many individual headers as necessary
    while (start_pos < headers_block_size) {
        end_pos = headers_block.find(":", start_pos);
        if (end_pos == std::string::npos)
            return (PARSE_ERROR);
        
        std::string key = headers_block.substr(start_pos, end_pos - start_pos);

		std::transform(key.begin(), key.end(), key.begin(), 
            static_cast<int(*)(int)>(std::tolower));
			
        start_pos = end_pos + 1;
        while (start_pos < headers_block_size &&
        std::isspace(static_cast<unsigned char>(headers_block[start_pos])))
            start_pos++;
        end_pos = headers_block.find("\r\n", start_pos);
        if (end_pos == std::string::npos)
            end_pos = headers_block_size;
            
        std::string value = headers_block.substr(start_pos, end_pos - start_pos);

        this->request.setHeader(key, value);
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
			return (CONTINUE);
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
			return (CONTINUE);
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
			return (CONTINUE);
        }
        case Request::DONE: {
            this->response = this->response.handle_request(this->request);
            
			this->clean_buffer_for_new_request();
            this->request.clear();
            this->request.state = Request::REQUEST_LINE;
            // new UPDATE
			std::string serialized = this->response.serialize();
            this->write_buffer += serialized;
            return (STOP);
        }
        case Request::ERROR: {
			LOG_ERROR("Connection: Request parsing error");
      		this->response = this->response.handle_error(400);
			this->clean_buffer_for_new_request();
			this->request.clear();
			this->request.state = Request::REQUEST_LINE;
			// new UPDATE
			std::string serialized = this->response.serialize();
			this->write_buffer += serialized;
            return (STOP);
        }
    }
	return (CONTINUE);
}

void Connection::clean_buffer_for_new_request() {
    std::string::size_type start_pos;

    start_pos = this->read_buffer.find("\r\n\r\n");
    start_pos += 4;

    if (this->request.headers.count("content-length"))
    {
        std::size_t content_length = static_cast<std::size_t>
            (std::strtoul(this->request.headers["content-length"].c_str(), NULL, 10));
        start_pos += content_length;

    }
    read_buffer.erase(0, start_pos);
}
