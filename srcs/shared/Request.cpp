#include "Request.hpp"

Request::Request(const Request &other) 
    : method(other.method), uri(other.uri),  version(other.version), 
      headers(other.headers), body(other.body), query_string(other.query_string), 
      state(other.state) {}

Request::Request() 
    : method(""), uri(""), version("HTTP/1.1"), state(REQUEST_LINE) {};

Request::~Request() {};

Request& Request::operator=(const Request &other) {
	if (this != &other) {
		this->method = other.method;
		this->uri = other.uri;
		this->path = other.path;
		this->version = other.version;
		this->headers = other.headers;
		this->body = other.body;
		this->query_string = other.query_string;
		this->state = other.state;
	}
	return *this;
}


std::string Request::getHeader(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end()) {
		return it->second;
	}
	return "";
}

void Request::setHeader(const std::string &key, const std::string &value) {
	headers[key] = value;
}

std::string Request::toString() const {
	std::ostringstream requestStream;

	std::string pathSTR = !path.empty() ? path : !uri.empty() ? uri.substr(0, uri.find('?')) : "/";
	
	requestStream << method << " " << pathSTR << " " << version << "\r\n";
	
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		requestStream << it->first << ": " << it->second << "\r\n";
	}
	requestStream << "\r\n" << body;
	return requestStream.str();
}

void Request::clear() {
	method.clear();
	uri.clear();
	path.clear();
	version = "HTTP/1.1";
	headers.clear();
	body.clear();
	query_string.clear();
	state = REQUEST_LINE;
}
