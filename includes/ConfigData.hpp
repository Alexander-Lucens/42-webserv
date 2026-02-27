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
	
	RedirectionPair() : status_code(0) {}
};

struct LocationConfig {
	std::string					path;
	std::string					root;
	std::vector<std::string>	index;
	std::vector<std::string>	methods;
	bool						autoindex;
	bool						upload_enabled;
;
	bool						auth_required;

	std::string                 cgi_path;
    std::string                 cgi_ext;

	RedirectionPair				redirection;

	LocationConfig() : autoindex(false), upload_enabled(false), auth_required(false) {}
};

/**
 * @brief ServerConfig structure
 * 
 * @see ports - vector<int> - all PORT related to this server configuration
 * 
 * @see host - <std::string> - host name, used in Response and ConfigParser Singleton to get one specific config data
 * 
 * @see root - <std::string> - root directory for this server, used in Response to find files and in ConfigParser to check if file exists
 *
 * @see server_names - vector<std::string> - all server names 
 * 
 * @see error_pages - map<int, std::string> - map with path to error page map[error_num] = path as string
 * 
 * @see client_max_body_size - <size_t> - limitation for sith of body if required
 * 
 * @see client_max_body_size - <size_t> - limitation for sith of body if required
 * 
 * @see redirection - RedirectionPair - struct with data for redirection
 * 
 * @see locations - map<std::string, LocationConfig> - map with path to location configuration map[path] = LocationConfig
 * 
 * @see active_sessions - map<std::string, map<std::string, std::string>> - map with active sessions, where key is session_id and value is map with session data (e.g. username)
 * 
 * @see serverPassword - <std::string> - password for authentication, used in Response to check credentials
 */
struct ServerConfig {
	std::vector<int>				ports;
	std::string						host;
	std::string						root;
	std::vector<std::string>		index;

	std::vector<std::string>		server_names;
	std::map<int, std::string>		error_pages;
	size_t							client_max_body_size;

	RedirectionPair					redirection;

	std::map<std::string, LocationConfig>	locations;
    std::map<std::string, std::map<std::string, std::string> > active_sessions;

	std::string serverPassword;

	ServerConfig() : client_max_body_size(0) {
		serverPassword = "UltraSecretPassword123!";
    }
};