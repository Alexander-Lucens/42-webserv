
#pragma once

#include <iostream>
#include <map>

class Request {
	public:
		std::string method;
		std::string uri;
		std::map<std::string, std::string> headers;
		std::string body;
};
