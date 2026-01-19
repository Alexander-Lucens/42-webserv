
#pragma once

#include <string>
#include <iostream>
#include <map>
#include <ctime>
#include <sstream>
#include "Request.hpp"

/**
 * @brief Represents an HTTP response.
 * @version HTTP version (e.g., HTTP/1.1).
 * @statusCode HTTP status code (e.g., 200, 400, 404, 500).
 * @see https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
 * @statusMessage Human-readable status message (e.g., OK, Not Found).
 * 
 * @headers Key-value pairs of headers (e.g., Content-Type: text/html).
 * @body Response body content. (eg. HTML, JSON, etc.)
 * 
 * @note Orthodox Canonical Form (OCF) methods are implemented.
 * 
 * @method setStatus: Sets statusCode and statusMessage.
 * @method setHeader: Sets a header key-value pair.
 */
class Response {
	private:

	public:
		int status_code;
		std::string html_body;
		// map - stores key-values pars like a dict (key: used to look up values, value: used for storing/retrieving)
		std::map<std::string, std::string> headers;
		std::string body;

		/* ----- OCF ----- */
		Response();
		~Response();
		Response(const Response &other);
		Response& operator=(const Response &other);
		/* ------------- */

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
