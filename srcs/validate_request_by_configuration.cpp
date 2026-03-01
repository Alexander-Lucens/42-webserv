#include "Response.hpp"
#include "Request.hpp"
#include "ConfigParser.hpp"
#include "FileHandler.hpp"
#include "Logger.hpp"
#include <sys/stat.h>

/*
 * @brief Validates a request against the configuration.
 * 
 * @param request The request to validate.
 * If before ther is no path and query string in request, 
 * it will be extracted from the URI and stored in request.path and request.query_string.
 * @return true if the request is invalid, false otherwise.
 */
int Response::validate_request_by_configuration(const Request &request) {
    if (!_config) {
        LOG_ERROR("No configuration found for request: " << request.uri);
        return 500;
    }
    
    std::string path = request.uri;
    size_t query_pos = path.find('?');
    if (query_pos != std::string::npos) {
        path = path.substr(0, query_pos);
    }
	// LOG_DEBUG("Request PATH: " << request.path << " URI: " << request.uri);
    
    std::string search_path = path;
    if (search_path.length() > 1 && search_path[search_path.length() - 1] == '/') {
        search_path = search_path.substr(0, search_path.length() - 1);
    }
    
    std::string best_match;
    bool exact_match = false;
    
    if (_config->locations.count(search_path)) {
        best_match = search_path;
        exact_match = true;
    } else if (_config->locations.count(path)) {
        best_match = path;
        exact_match = true;
    } else {
        std::map<std::string, LocationConfig>::const_iterator it;
        for (it = _config->locations.begin(); it != _config->locations.end(); ++it) {
            if (search_path.find(it->first) == 0) {
                if (best_match.empty() || it->first.length() > best_match.length()) {
                    best_match = it->first;
                }
            }
        }
    }
    
    if (best_match.empty()) {
        LOG_WARNING("No matching location found for URI: " << request.uri);
        _conf_location_path = "";
        return 404;
    }
    
    _conf_location_path = best_match;
    // LOG_DEBUG("CONFIG VALIDATION. Path: " << _conf_location_path << " uri: " << request.uri);
    
    if (_config->locations.count(_conf_location_path)) {
        const std::vector<std::string>& methods = _config->locations.at(_conf_location_path).methods;
        if (!methods.empty() && std::find(methods.begin(), methods.end(), request.method) == methods.end()) {
            if (!exact_match && _conf_location_path != "/") {
                LOG_WARNING("Method " << request.method << " not allowed for location: " << _conf_location_path);
                return 404;
            }
            return 405;
        }
    }

	if (_config->locations.at(_conf_location_path).auth_required == true) {
		LOG_WARNING("REQUIRED AUTHENTICATION FOR LOCATION: " << _conf_location_path);

        const LocationConfig& loc = _config->locations.at(_conf_location_path);
        
        if (loc.auth_required) {
            std::string session_id = request.getCookie("session_id");
            if (session_id.empty()) {
                LOG_WARNING("Authentication required, but no session_id cookie for location: " << _conf_location_path);
                return 401;
            }
            if (!_config->active_sessions.count(session_id)) {
                LOG_WARNING("Invalid session ID: " << session_id);
                return 403;
            }
            std::string username = _config->active_sessions.at(session_id).at("username");
            LOG_INFO("Authenticated user: " << username << " for location: " << _conf_location_path);
        }
    }    
	// LOG_INFO("CONFIG VALIDATION PASSED. Path: " << _conf_location_path << " uri: " << request.uri);
    return 0;
}
