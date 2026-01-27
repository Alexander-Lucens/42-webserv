

#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <map>
#include <ctime>
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
		
	private:
		static const std::string BASE_DIR;
};