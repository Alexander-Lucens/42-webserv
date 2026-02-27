
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

		std::string			generate_error_page(int error_code);

		
		std::string			parse_form_data_value(const std::string &body, const std::string &key);

		// ???? EXIST as return but didnt find where its declared
		// Response			handle_get_cgi(const Request &request, Response &response, Language lang);
    	// Response			handle_post_cgi(const Request &request, Response &response, Language lang);
		/* ------------------------------------------------ */

		// all things relate to check configuration
		int				validate_request_by_configuration(const Request &request);
		
	public:
		std::string							_conf_location_path;
		std::map<std::string, std::string>	_cookies;
		std::string							version; 
		const ServerConfig*					_config;    

		/* ----- OCF ----- */
		Response();
		Response(const Response &other);
		Response& operator=(const Response &other);
		~Response();
		/* ------------- */

		// Config data
		void set_config(const Request &request);

		// Requests
		Response 		handle_get(const Request &request);
		Response 		handle_post(const Request &request);
		Response		handle_delete(const Request &request);
		Response		handle_redirect(const Request& request);


		Response 		handle_error(const int error_code);
		Response 		handle_request(const Request &request);
		Response 		response_body(const int &status_code, const std::string &body);

		// Setter
		void 			set_status(int status_code);
		void 			set_header(const std::string &key, const std::string &value);
		void 			set_body(const std::string &html_body);

		// Cookies
		void			set_cookie(const std::string &name, const std::string &value, int max_age, const std::string &path);
		void			set_session_cookie(const std::string &name, const std::string &value);
		std::string		generate_session_id();

		// CGI
		// Response		handle_get_cgi(const Request &request, Response &response, Language lang);
    	// Response		handle_post_cgi(const Request &request, Response &response, Language lang);

		// Serializer 
		std::string		serialize();

};

std::string get_html_header(const std::string &title);
std::string get_html_footer();
std::string get_inline_css();
std::string generate_signin_page();
std::string generate_success_page(const std::string &title, const std::string &message);
