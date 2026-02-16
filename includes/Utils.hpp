#pragma once

#include <string>
#include <ctime>

#define NEW_LINE		"\r\n"
#define BLANK_LINE		"\r\n\r\n"
#define SERVER			"webserv/1.0"
#define MAX_FILE_SIZE	(5 * 1024 * 1024) 

class Utils {
	public:
	static std::string get_http_date();
	static std::string extract_boundary(const std::string& content_type);
	static std::string lower_case(std::string str);
	static std::string upper_case(std::string str);
};
