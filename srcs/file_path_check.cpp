#include "Response.hpp"
#include "Request.hpp"
#include "ConfigParser.hpp"
#include "FileHandler.hpp"
#include "Logger.hpp"
#include <sys/stat.h>

std::string Response::file_path_check(const std::string &uri)
{
    if (!_config) return "." + uri;

    std::string root = _config->root;
    std::string location_path = _conf_location_path;
    
    if (!root.empty() && root[root.length() - 1] != '/') {
        root += "/";
    }
    
    if (_config->locations.count(location_path) && !_config->locations.at(location_path).root.empty()) {
        root = _config->locations.at(location_path).root;
        if (!root.empty() && root[root.length() - 1] != '/') {
            root += "/";
        }
        
        const LocationConfig& loc = _config->locations.at(location_path);

        if (loc.upload_enabled == true) {
            LOG_DEBUG("Storage directory detected for location: " << location_path);
            return root;
        }

        std::string remaining_uri = uri;
        if (uri.find(location_path) == 0) {
            remaining_uri = uri.substr(location_path.length());
            if (remaining_uri.empty()) {
                remaining_uri = "/";
            }
        }
        
        if (!remaining_uri.empty() && remaining_uri[0] == '/') {
            remaining_uri = remaining_uri.substr(1);
        }
        
        std::string file_path = root + remaining_uri;
        
        // const LocationConfig& loc = _config->locations.at(location_path);

        if (loc.upload_enabled == true) {
            size_t pos;
            while ((pos = file_path.find("//")) != std::string::npos) {
                file_path.erase(pos, 1);
            }
            LOG_DEBUG("Storage directory detected for location: " << location_path);
            return file_path;
        }
        
        if (file_path.length() > 0 && file_path[file_path.length() - 1] == '/') {
            file_path += "index.html";
        }
        
        size_t pos;
        while ((pos = file_path.find("//")) != std::string::npos) {
            file_path.erase(pos, 1);
        }
        return file_path;
    }
    
    std::string uri_part = uri;
    if (!uri_part.empty() && uri_part[0] == '/') {
        uri_part = uri_part.substr(1);
    }
    
    std::string file_path = root + uri_part;
    
    if (_config->locations.count(location_path)) {
        const LocationConfig& loc = _config->locations.at(location_path);
        if (loc.upload_enabled) {
            size_t pos;
            while ((pos = file_path.find("//")) != std::string::npos) {
                file_path.erase(pos, 1);
            }
            LOG_DEBUG("Storage directory detected for location: " << location_path);
            return file_path;
        }
    }
    
    if (file_path.length() > 0 && file_path[file_path.length() - 1] == '/') {
        file_path += "index.html";
    } else if (FileHandler::is_directory(file_path)) {
        file_path += "/index.html";
    }
    
    size_t pos;
    while ((pos = file_path.find("//")) != std::string::npos) {
        file_path.erase(pos, 1);
    }
    
    return file_path;
}