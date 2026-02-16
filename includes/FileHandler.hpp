

#pragma once

#include <string>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <map>
#include <ctime>
#include <vector>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include "Request.hpp"


#define NEW_LINE		"\r\n"
#define BLANK_LINE		"\r\n\r\n"
#define SERVER			"webserv/1.0"
#define MAX_FILE_SIZE	(5 * 1024 * 1024) 


class FileHandler {
	public:
		static bool			file_exists(const std::string &path);
		static bool 		is_directory(const std::string &path);
		static bool 		is_readable(const std::string &path);
		static std::string 	load_file(const std::string &path);
		static std::string 	get_content_type(const std::string &path);
		static std::string 	find_content_type(const std::string &content_file_path);
		static bool			save_uploaded_file(const std::string& file_path, const std::string& content);
		static std::string	handle_autoindex(const std::string &normalized_html_path, const std::string &directory_path);
		static std::string 	html_escape(const std::string& input);
		static std::string	get_filename_from_multipart(const std::string &body);
		static std::string	get_file_content(const std::string &body, const std::string &boundary);
		static std::string	extract_form_data(const std::string &body, const std::string &field_name);
};