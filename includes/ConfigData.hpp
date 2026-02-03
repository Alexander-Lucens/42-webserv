#pragma once

#include <string>
#include <cstddef>
#include <vector>
#include <map>
#include <iostream>

struct LocationConfig {
	std::string					path;
	std::string					root;
	std::vector<std::string>	index;
	std::vector<std::string>	methods;
	bool						autoindex;
	std::string                 cgi_path;
    std::string                 cgi_ext;

	LocationConfig() : autoindex(false) {}
};

struct ServerConfig {
	int									port;
	std::string							host;
	std::vector<std::string>			server_names;
	std::map<int, std::string>			error_pages;
	size_t								client_max_body_size;

	std::string							root;
	std::vector<std::string>			index;

	std::map<std::string, LocationConfig>	locations;

	ServerConfig() : 
		port(80),
		host("127.0.0.1"),
		client_max_body_size(1024*1024) {}
};