#include "../includes/ServerConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

ServerConfig::ServerConfig() {
}

ServerConfig::~ServerConfig() {
}

void ServerConfig::loadDefault() {
	ServerBlock server;
	server.host = "0.0.0.0";
	server.port = 8080;
	server.clientMaxBodySize = 1024 * 1024; // 1MB
	
	ServerBlock::RouteConfig route;
	route.path = "/";
	route.allowedMethods.push_back("GET");
	route.root = "./www";
	route.index = "index.html";
	route.directoryListing = false;
	
	server.routes.push_back(route);
	servers_.push_back(server);
}

bool ServerConfig::loadFromFile(const std::string& filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		return false;
	}
	
	// TODO: Person 3 should implement full config file parsing
	// For now, just load default
	loadDefault();
	
	return true;
}

const std::vector<ServerBlock>& ServerConfig::getServerBlocks() const {
	return servers_;
}

const ServerBlock* ServerConfig::findServer(const std::string& host, int port) const {
	for (size_t i = 0; i < servers_.size(); ++i) {
		if (servers_[i].port == port) {
			// Check host match
			if (servers_[i].host == "0.0.0.0" || servers_[i].host == host) {
				return &servers_[i];
			}
		}
	}
	return NULL;
}

const ServerBlock* ServerConfig::findServerByName(const std::string& serverName) const {
	for (size_t i = 0; i < servers_.size(); ++i) {
		for (size_t j = 0; j < servers_[i].serverNames.size(); ++j) {
			if (servers_[i].serverNames[j] == serverName) {
				return &servers_[i];
			}
		}
	}
	return NULL;
}

bool ServerConfig::parseServerBlock(std::istream& is, ServerBlock& server) {
	// TODO: Person 3 should implement full parsing
	(void)is;
	(void)server;
	return false;
}

bool ServerConfig::parseRoute(std::istream& is, ServerBlock::RouteConfig& route) {
	// TODO: Person 3 should implement full parsing
	(void)is;
	(void)route;
	return false;
}

std::string ServerConfig::trim(const std::string& str) const {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return "";
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, last - first + 1);
}

std::vector<std::string> ServerConfig::split(const std::string& str, char delim) const {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (std::getline(iss, token, delim)) {
		tokens.push_back(trim(token));
	}
	return tokens;
}
