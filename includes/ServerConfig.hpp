#pragma once

#include <string>
#include <vector>
#include <map>

// Person 3: Server configuration
// Parses configuration file and provides server settings
struct ServerBlock {
	std::string host;
	int port;
	std::vector<std::string> serverNames;
	std::map<int, std::string> errorPages; // status code -> file path
	size_t clientMaxBodySize;
	
	// Route configurations
	struct RouteConfig {
		std::string path;
		std::vector<std::string> allowedMethods;
		std::string root; // Directory root
		std::string index; // Default file
		bool directoryListing;
		std::string cgiExtension; // e.g., ".php"
		std::string cgiPath; // Path to CGI executable
		std::string uploadDir; // Where to save uploaded files
		std::string redirect; // HTTP redirect URL
	};
	
	std::vector<RouteConfig> routes;
};

class ServerConfig {
	public:
		ServerConfig();
		~ServerConfig();
		
		// Load configuration from file
		// Returns true on success
		bool loadFromFile(const std::string& filename);
		
		// Load default configuration
		void loadDefault();
		
		// Get all server blocks
		const std::vector<ServerBlock>& getServerBlocks() const;
		
		// Find server block by host:port
		const ServerBlock* findServer(const std::string& host, int port) const;
		
		// Find server block by server_name
		const ServerBlock* findServerByName(const std::string& serverName) const;
		
	private:
		std::vector<ServerBlock> servers_;
		
		// Parsing helpers
		bool parseServerBlock(std::istream& is, ServerBlock& server);
		bool parseRoute(std::istream& is, ServerBlock::RouteConfig& route);
		std::string trim(const std::string& str) const;
		std::vector<std::string> split(const std::string& str, char delim) const;
};
