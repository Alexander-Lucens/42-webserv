#include "Response.hpp"

Response::Response(const Response &other) 
    : version(other.version), statusCode(other.statusCode), statusMessage(other.statusMessage), 
      headers(other.headers), body(other.body) {}

Response::Response() 
    : statusCode(200), statusMessage("OK"), version("HTTP/1.1") {}

Response::~Response() {}

Response& Response::operator=(const Response &other) {
	if (this != &other) {
		this->version = other.version;
		this->statusCode = other.statusCode;
		this->statusMessage = other.statusMessage;
		this->headers = other.headers;
		this->body = other.body;
	}
	return *this;
}

void Response::setHeader(const std::string &key, const std::string &value) {
	headers[key] = value;
}

void Response::setStatus(int code, const std::string &message) {
	statusCode = code;
	statusMessage = message;
}

std::string Response::toString() const {
	std::ostringstream responseStream;

	responseStream << version << " " << statusCode << " " << statusMessage << "\r\n";

	if (headers.find("Content-Length") == headers.end()) {
		responseStream << "Content-Length: " << body.size() << "\r\n";
	}
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		responseStream << it->first << ": " << it->second << "\r\n";
	}
	responseStream << "\r\n";
	responseStream << body;
	return responseStream.str();
}

void Response::clear() {
	headers.clear();
	body.clear();
	version = "HTTP/1.1";
	statusCode = 200;
	statusMessage = "OK";
}

