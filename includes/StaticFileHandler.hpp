#pragma once

#include "Handler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <string>

// Person 3: Example handler for serving static files
class StaticFileHandler : public Handler {
	public:
		StaticFileHandler(const std::string& rootDir, 
						 const std::string& indexFile = "index.html",
						 bool directoryListing = false);
		virtual ~StaticFileHandler();
		
		virtual Response handle(const Request& request);
		virtual bool canHandle(const Request& request) const;
		
	private:
		std::string rootDir_;
		std::string indexFile_;
		bool directoryListing_;
		
		// Helper methods
		std::string resolvePath(const std::string& uri) const;
		bool isDirectory(const std::string& path) const;
		bool fileExists(const std::string& path) const;
		std::string readFile(const std::string& path) const;
		std::string generateDirectoryListing(const std::string& path, const std::string& uri) const;
		std::string getMimeType(const std::string& filename) const;
};
