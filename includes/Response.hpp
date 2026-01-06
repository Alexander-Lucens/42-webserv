
#pragma once

#include <iostream>
#include <map>

class Response {
	public:
		int status;
		std::string body;
		std::map<std::string, std::string> headers;

		std::string toString();
};
