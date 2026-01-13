#include "../includes/HTTPParser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

HTTPParser::HTTPParser() : currentState_(Request::PARSING_REQUEST_LINE),
							expectedBodyLength_(0) {
}

HTTPParser::~HTTPParser() {
}

void HTTPParser::reset() {
	currentState_ = Request::PARSING_REQUEST_LINE;
	expectedBodyLength_ = 0;
	errorMessage_.clear();
}

bool HTTPParser::isComplete() const {
	return currentState_ == Request::PARSING_COMPLETE;
}

std::string HTTPParser::getError() const {
	return errorMessage_;
}

ssize_t HTTPParser::parse(Buffer& buffer, Request& request) {
	ssize_t totalConsumed = 0;
	
	while (currentState_ != Request::PARSING_COMPLETE && 
		   currentState_ != Request::PARSING_ERROR &&
		   !buffer.empty()) {
		
		ssize_t consumed = 0;
		
		switch (currentState_) {
			case Request::PARSING_REQUEST_LINE: {
				std::string line = readLine(buffer);
				if (!line.empty()) {
					if (parseRequestLine(line, request)) {
						currentState_ = Request::PARSING_HEADERS;
						consumed = line.size() + 2; // +2 for \r\n
					} else {
						currentState_ = Request::PARSING_ERROR;
						errorMessage_ = "Invalid request line";
						return -1;
					}
				} else {
					// Need more data
					return totalConsumed;
				}
				break;
			}
			
			case Request::PARSING_HEADERS: {
				std::string line = readLine(buffer);
				if (line.empty()) {
					// Empty line means end of headers
					currentState_ = Request::PARSING_BODY;
					consumed = 2; // \r\n
					
					// Check if we need to read body
					std::string contentLength = request.getHeader("content-length");
					if (!contentLength.empty()) {
						std::istringstream iss(contentLength);
						iss >> expectedBodyLength_;
					}
					
					if (expectedBodyLength_ == 0) {
						// No body expected
						currentState_ = Request::PARSING_COMPLETE;
						request.parseState = Request::PARSING_COMPLETE;
					}
				} else {
					if (parseHeaderLine(line, request)) {
						consumed = line.size() + 2; // +2 for \r\n
					} else {
						currentState_ = Request::PARSING_ERROR;
						errorMessage_ = "Invalid header line";
						return -1;
					}
				}
				break;
			}
			
			case Request::PARSING_BODY: {
				if (parseBody(buffer, request)) {
					currentState_ = Request::PARSING_COMPLETE;
					request.parseState = Request::PARSING_COMPLETE;
					consumed = expectedBodyLength_;
				} else {
					// Need more data
					return totalConsumed;
				}
				break;
			}
			
			default:
				return -1;
		}
		
		totalConsumed += consumed;
	}
	
	return totalConsumed;
}

std::string HTTPParser::readLine(Buffer& buffer) {
	std::string line;
	char c;
	
	while (buffer.size() > 0) {
		if (buffer.peek(&c, 1, line.size()) == 0) {
			break;
		}
		
		if (c == '\n') {
			// Found newline, check if previous was \r
			if (!line.empty() && line[line.size() - 1] == '\r') {
				line.erase(line.size() - 1);
			}
			return line;
		}
		
		line += c;
		
		// Safety check
		if (line.size() > 8192) { // Max header line size
			return "";
		}
	}
	
	return ""; // Incomplete line
}

bool HTTPParser::parseRequestLine(const std::string& line, Request& request) {
	std::istringstream iss(line);
	std::string method, uri, version;
	
	if (!(iss >> method >> uri >> version)) {
		return false;
	}
	
	// Validate method
	if (!isValidMethod(method)) {
		return false;
	}
	
	// Validate HTTP version
	if (!isValidHTTPVersion(version)) {
		return false;
	}
	
	// Parse query string from URI
	size_t queryPos = uri.find('?');
	if (queryPos != std::string::npos) {
		request.queryString = uri.substr(queryPos + 1);
		uri = uri.substr(0, queryPos);
	}
	
	request.method = method;
	request.uri = uri;
	request.httpVersion = version;
	
	return true;
}

bool HTTPParser::parseHeaderLine(const std::string& line, Request& request) {
	size_t colonPos = line.find(':');
	if (colonPos == std::string::npos || colonPos == 0) {
		return false;
	}
	
	std::string key = line.substr(0, colonPos);
	std::string value = line.substr(colonPos + 1);
	
	// Trim whitespace
	while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) {
		value.erase(0, 1);
	}
	
	// Convert key to lowercase for case-insensitive lookup
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	
	request.headers[key] = value;
	return true;
}

bool HTTPParser::parseBody(Buffer& buffer, Request& request) {
	if (expectedBodyLength_ == 0) {
		return true;
	}
	
	if (buffer.size() >= expectedBodyLength_) {
		request.body = buffer.readString(expectedBodyLength_);
		return true;
	}
	
	return false; // Need more data
}

bool HTTPParser::isValidMethod(const std::string& method) const {
	return method == "GET" || method == "POST" || method == "DELETE" ||
		   method == "PUT" || method == "HEAD" || method == "OPTIONS";
}

bool HTTPParser::isValidHTTPVersion(const std::string& version) const {
	return version == "HTTP/1.0" || version == "HTTP/1.1";
}
