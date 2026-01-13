#include "../includes/Request.hpp"
#include <algorithm>
#include <cctype>

Request::Request() : parseState(PARSING_REQUEST_LINE) {
}

Request::~Request() {
}

std::string Request::getHeader(const std::string& key) const {
	std::string lowerKey = key;
	std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
	
	std::map<std::string, std::string>::const_iterator it = headers.find(lowerKey);
	if (it != headers.end()) {
		return it->second;
	}
	return "";
}

bool Request::hasHeader(const std::string& key) const {
	std::string lowerKey = key;
	std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
	return headers.find(lowerKey) != headers.end();
}

void Request::clear() {
	method.clear();
	uri.clear();
	httpVersion.clear();
	headers.clear();
	body.clear();
	queryString.clear();
	parseState = PARSING_REQUEST_LINE;
}
