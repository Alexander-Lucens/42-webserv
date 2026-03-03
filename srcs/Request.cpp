#include "Request.hpp"
#include "Response.hpp"


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

bool Request::thereisHeader(const std::string &key) const {
std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end()) {
		return (true);
	}
	else
		return (false);
}

std::map<std::string, std::string> Request::getAllHeaders() const {
	return headers;
}

std::string Request::getHeader(const std::string &key) const
{	
	std::string lower_key = key;
	std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(),
		static_cast<int(*)(int)>(std::tolower));

	std::map<std::string, std::string>::const_iterator it = headers.find(lower_key);
	if (it != headers.end())
		return it->second;
	return "";
}

void Request::setHeader(const std::string &key, const std::string &value) {
	headers[key] = value;
}

void Request::clear() {
	method.clear();
	uri.clear();
	path.clear();
	version.clear();
	headers.clear();
	body.clear();
	query_string.clear();
	state = REQUEST_LINE;
}

std::string Request::toString() const {
	std::string result;
	result += method + " " + uri + " " + version + "\r\n";
	result += "uri: " + uri + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		result += it->first + ": " + it->second + "\r\n";
	}
	result += "\r\n";
	result += body;
	return result;
}

std::string Request::getCookie(const std::string &key) const {
    std::map<std::string, std::string>::const_iterator it = _cookies.find(key);
    if (it != _cookies.end())
        return it->second;
    return "";
}

void Request::parseCookies() {
    std::string cookie_header = getHeader("Cookie");
    if (cookie_header.empty())
        return;
    
    std::istringstream iss(cookie_header);
    std::string pair;
    
    while (std::getline(iss, pair, ';')) {
        size_t start = pair.find_first_not_of(" \t");
        if (start != std::string::npos)
            pair = pair.substr(start);
        
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            std::string name = pair.substr(0, eq_pos);
            std::string value = pair.substr(eq_pos + 1);
            _cookies[name] = value;
        }
    }
}