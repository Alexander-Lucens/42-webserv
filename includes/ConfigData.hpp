#pragma once

#include <string>
#include <cstddef>
#include <vector>
#include <map>
#include <iostream>

struct RedirectionPair
{
	std::string from;
	std::string to;
	int status_code;
};

struct LocationConfig {
	std::string					path;
	std::string					root;
	std::vector<std::string>	index;
	std::vector<std::string>	methods;
	bool						autoindex;
	std::string                 cgi_path;
    std::string                 cgi_ext;

	RedirectionPair				redirection;

	LocationConfig() : autoindex(false) {}
};

/**
 * @brief ServerConfig structure
 * 
 * @see ports - vector<int> - all PORT related to this server configuration
 * 
 * @see host - <std::string> - host name, used in Response and ConfigParser Singleton to get one specific config data
 * 
 * @see server_names - vector<std::string> - all server names 
 * 
 * @see error_pages - map<int, std::string> - map with path to error page map[error_num] = path as string
 * 
 * @see client_max_body_size - <size_t> - limitation for sith of body if required
 */
struct ServerConfig {
	std::vector<int>				ports;
	std::string						host;
	std::vector<std::string>		server_names;
	std::map<int, std::string>		error_pages;
	size_t							client_max_body_size;

	RedirectionPair					redirection;

	std::string						root;
	std::vector<std::string>		index;

	std::map<std::string, LocationConfig>	locations;

	// ServerConfig() : 
	// 	port(80),
	// 	host("127.0.0.1"),
	// 	client_max_body_size(1024*1024) {}
};