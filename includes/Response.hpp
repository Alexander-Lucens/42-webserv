
#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <map>
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include "FileHandler.hpp"
#include "Utils.hpp"
#include "ConfigParser.hpp"
#include "Logger.hpp"
#include <sys/stat.h>

#define NEW_LINE		"\r\n"
#define BLANK_LINE		"\r\n\r\n"
#define SERVER			"webserv/1.0"

enum Language {PYTHON, RUST};

class Request; 

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
		int									_status_code;
		std::string							_html_body;
		// map - stores key-values pars like a dict (key: used to look up values, value: used for storing/retrieving)
		std::map<std::string, std::string>	_headers;
		std::string							_method;
		
		std::string							_request_uri; // Need to think about naming and for what it to be used
		


		// Helper 
		std::string			reason_message(int status_code);
		Response			handle_post_submit(const Request &request);
		Response			handle_post_upload(const Request &request);
		std::string 		file_path_check(const std::string &uri);
		Response			handle_directory(const std::string& uri, std::string& file_path);
		void				set_download_header(const std::string &path);
		void				set_method(const Request &request);
		int					validate_file_path(const std::string& file_path);
		Response			handle_special_routes(const std::string& uri);
		int					validate_file_writable(const std::string& file_path);

		std::string			generate_error_html(int error_code);


		// all things relate to check configuration
		bool				validate_request_by_configuration(const Request &request);
		
	public:
		std::string							_conf_location_path;

		std::string							version; 
		// _configuration data
		const ServerConfig*					_config;

		/* ----- OCF ----- */
		Response();
		~Response();
		Response(const Response &other);
		Response& operator=(const Response &other);
		/* ------------- */

		// Config data
		void set_config(const Request &request);

		// Requests
		Response 		handle_get(const Request &request);
		Response 		handle_post(const Request &request);
		Response		handle_delete(const Request &request);
		Response		handle_redirect();


		Response 		handle_error(const int error_code);
		Response 		handle_request(const Request &request);
		Response 		response_body(const int &status_code, const std::string &body);

		// Setter
		void 			set_status(int status_code);
		void 			set_header(const std::string &key, const std::string &value);
		void 			set_body(const std::string &html_body);

		// Serializer 
		std::string		serialize();

};
