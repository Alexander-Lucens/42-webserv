
#pragma once

#include <string>
#include <map>
#include <iostream>
#include <sstream>

/**
 * @brief Class for handling HTTP requests
 * @method GET, POST, DELETE, etc.
 * @uri /index.html, /api/data, etc.
 * @path part of the URI before '?' - need to be filled in Parser!
 * @version HTTP/1.1, HTTP/2, etc. more likely only 1.1
 * @headers key-value pairs of headers as 'Host: localhost, Content-Type: text/html'
 * @body request body
 * @query_string part of the URI after '?'
 * 
 * @state enum to track the state of the request parsing
 * 
 * @note OCF: Orthodox Canonical Form implemented
 * 
 * Clear method to reset the request object
 * getHeader method to retrieve header values by key
 */
class Request {
	public:
		std::string method;
		std::string uri;
		std::string path;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;
		std::string query_string;

		enum RequestState {
			REQUEST_LINE,
			REQUEST_HEADERS,
			REQUEST_BODY,
			DONE,
			ERROR
		};
		RequestState state;

		/* ----- OCF ----- */
		Request();
		~Request();
		// Request(const Request &other);
		// Request& operator=(const Request &other);
		/* ------------- */

		void clear();

		std::string getHeader(const std::string &key) const;
		void setHeader(const std::string &key, const std::string &value);
		std::string toString() const;
};
