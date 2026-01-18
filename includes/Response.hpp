
#pragma once

#include <iostream>
#include <map>
#include <ctime>
#include <sstream>
#include "Request.hpp"

class Response {
	private:

	public:
		int status_code;
		std::string html_body;
		// map - stores key-values pars like a dict (key: used to look up values, value: used for storing/retrieving)
		std::map<std::string, std::string> headers;


		// Requests
		Response handle_get(const Request& request);
		Response handle_post(const Request& request);
		Response handle_delete(const Request& request);
		Response handle_error(const int error_code);
		Response handle_request(const Request &request);

		// Serializer 
		std::string serialize(const Response &response);


		// Helper 
		std::string getHttpDate();
		std::string toString();
		std::string reason_message(int code);

};
