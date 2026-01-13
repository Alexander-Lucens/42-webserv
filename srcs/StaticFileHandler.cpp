#include "../includes/StaticFileHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>

StaticFileHandler::StaticFileHandler(const std::string& rootDir,
									 const std::string& indexFile,
									 bool directoryListing)
	: rootDir_(rootDir), indexFile_(indexFile), directoryListing_(directoryListing) {
}

StaticFileHandler::~StaticFileHandler() {
}

Response StaticFileHandler::handle(const Request& request) {
	Response response;
	
	if (request.method != "GET" && request.method != "HEAD") {
		response.setStatus(405, "Method Not Allowed");
		response.setHeader("Allow", "GET, HEAD");
		return response;
	}
	
	std::string filePath = resolvePath(request.uri);
	
	if (isDirectory(filePath)) {
		// Try index file
		std::string indexPath = filePath + "/" + indexFile_;
		if (fileExists(indexPath)) {
			filePath = indexPath;
		} else if (directoryListing_) {
			// Generate directory listing
			response.setStatus(200, "OK");
			response.body = generateDirectoryListing(filePath, request.uri);
			response.setHeader("Content-Type", "text/html");
			return response;
		} else {
			response.setStatus(403, "Forbidden");
			return response;
		}
	}
	
	if (!fileExists(filePath)) {
		response.setStatus(404, "Not Found");
		return response;
	}
	
	// Read and serve file
	std::string content = readFile(filePath);
	response.setStatus(200, "OK");
	response.body = content;
	response.setHeader("Content-Type", getMimeType(filePath));
	response.setHeader("Content-Length", "");
	
	// Convert to string for Content-Length
	std::ostringstream oss;
	oss << content.size();
	response.setHeader("Content-Length", oss.str());
	
	return response;
}

bool StaticFileHandler::canHandle(const Request& request) const {
	return request.method == "GET" || request.method == "HEAD";
}

std::string StaticFileHandler::resolvePath(const std::string& uri) const {
	std::string path = rootDir_ + uri;
	// TODO: Add path sanitization to prevent directory traversal
	return path;
}

bool StaticFileHandler::isDirectory(const std::string& path) const {
	struct stat st;
	if (stat(path.c_str(), &st) == 0) {
		return S_ISDIR(st.st_mode);
	}
	return false;
}

bool StaticFileHandler::fileExists(const std::string& path) const {
	struct stat st;
	return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

std::string StaticFileHandler::readFile(const std::string& path) const {
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		return "";
	}
	
	std::ostringstream oss;
	oss << file.rdbuf();
	return oss.str();
}

std::string StaticFileHandler::generateDirectoryListing(const std::string& path, const std::string& uri) const {
	std::ostringstream html;
	html << "<html><head><title>Index of " << uri << "</title></head><body>";
	html << "<h1>Index of " << uri << "</h1><hr><ul>";
	
	DIR* dir = opendir(path.c_str());
	if (dir) {
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			if (std::strcmp(entry->d_name, ".") != 0) {
				std::string name = entry->d_name;
				std::string link = uri;
				if (link[link.size() - 1] != '/') {
					link += "/";
				}
				link += name;
				html << "<li><a href=\"" << link << "\">" << name << "</a></li>";
			}
		}
		closedir(dir);
	}
	
	html << "</ul><hr></body></html>";
	return html.str();
}

std::string StaticFileHandler::getMimeType(const std::string& filename) const {
	size_t dotPos = filename.find_last_of('.');
	if (dotPos == std::string::npos) {
		return "application/octet-stream";
	}
	
	std::string ext = filename.substr(dotPos + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	
	if (ext == "html" || ext == "htm") return "text/html";
	if (ext == "css") return "text/css";
	if (ext == "js") return "application/javascript";
	if (ext == "json") return "application/json";
	if (ext == "png") return "image/png";
	if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
	if (ext == "gif") return "image/gif";
	if (ext == "txt") return "text/plain";
	
	return "application/octet-stream";
}
