#pragma once

#include <string>
#include <ctime>
#include <cerrno>
#include "Logger.hpp"


#define NEW_LINE		"\r\n"
#define BLANK_LINE		"\r\n\r\n"
#define SERVER			"webserv/1.0"

class Utils {
	public:
	static std::string	get_http_date();
	static std::string	extract_boundary(const std::string& content_type);
	static std::string	lower_case(std::string str);
	static std::string	upper_case(std::string str);
	static std::string	dash_to_underscore(std::string str);
	static int			get_errno_code();
	static bool			is_valid_uri(const std::string& uri);

};
