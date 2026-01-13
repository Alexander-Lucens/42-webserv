#include "../includes/Response.hpp"
#include <sstream>
#include <map>

Response::Response() : statusCode(200), statusMessage("OK") {
}

Response::~Response() {
}

void Response::setStatus(int code, const std::string& message) {
	statusCode = code;
	if (message.empty()) {
		// Default status messages
		switch (code) {
			case 200: statusMessage = "OK"; break;
			case 201: statusMessage = "Created"; break;
			case 204: statusMessage = "No Content"; break;
			case 301: statusMessage = "Moved Permanently"; break;
			case 302: statusMessage = "Found"; break;
			case 400: statusMessage = "Bad Request"; break;
			case 403: statusMessage = "Forbidden"; break;
			case 404: statusMessage = "Not Found"; break;
			case 405: statusMessage = "Method Not Allowed"; break;
			case 413: statusMessage = "Payload Too Large"; break;
			case 500: statusMessage = "Internal Server Error"; break;
			case 501: statusMessage = "Not Implemented"; break;
			case 505: statusMessage = "HTTP Version Not Supported"; break;
			default: statusMessage = "Unknown"; break;
		}
	} else {
		statusMessage = message;
	}
}

void Response::setHeader(const std::string& key, const std::string& value) {
	headers[key] = value;
}

std::string Response::getHeader(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end()) {
		return it->second;
	}
	return "";
}

void Response::clear() {
	statusCode = 200;
	statusMessage = "OK";
	headers.clear();
	body.clear();
}

std::string Response::toString() const {
	std::ostringstream oss;
	
	// Status line
	oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
	
	// Headers
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		 it != headers.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	
	// Content-Length if not set
	if (headers.find("Content-Length") == headers.end() && !body.empty()) {
		oss << "Content-Length: " << body.size() << "\r\n";
	}
	
	// Empty line before body
	oss << "\r\n";
	
	// Body
	oss << body;
	
	return oss.str();
}

bool Response::isReady() const {
	return statusCode > 0;
}
