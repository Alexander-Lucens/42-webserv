#include "../includes/Router.hpp"
#include <algorithm>

Router::Router() : defaultHandler_(NULL) {
}

Router::~Router() {
	clear();
}

void Router::addRoute(const std::string& pattern, 
					  const std::vector<std::string>& methods,
					  Handler* handler) {
	Route route;
	route.pattern = pattern;
	route.methods = methods;
	route.handler = handler;
	routes_.push_back(route);
}

Response Router::route(const Request& request) {
	// Try to find matching route
	for (size_t i = 0; i < routes_.size(); ++i) {
		if (matchPattern(routes_[i].pattern, request.uri) &&
			isMethodAllowed(routes_[i], request.method)) {
			return routes_[i].handler->handle(request);
		}
	}
	
	// No route found, use default handler or return 404
	if (defaultHandler_) {
		return defaultHandler_->handle(request);
	}
	
	// Default 404 response
	Response response;
	response.setStatus(404, "Not Found");
	response.body = "<html><body><h1>404 Not Found</h1></body></html>";
	response.setHeader("Content-Type", "text/html");
	return response;
}

void Router::setDefaultHandler(Handler* handler) {
	defaultHandler_ = handler;
}

void Router::clear() {
	for (size_t i = 0; i < routes_.size(); ++i) {
		if (routes_[i].handler) {
			delete routes_[i].handler;
		}
	}
	routes_.clear();
	if (defaultHandler_) {
		delete defaultHandler_;
		defaultHandler_ = NULL;
	}
}

bool Router::matchPattern(const std::string& pattern, const std::string& uri) const {
	// Simple pattern matching
	// Supports exact match and wildcard suffix (e.g., "/static/*")
	
	if (pattern == uri) {
		return true;
	}
	
	// Check for wildcard pattern
	if (pattern.size() > 1 && pattern[pattern.size() - 1] == '*') {
		std::string prefix = pattern.substr(0, pattern.size() - 1);
		if (uri.size() >= prefix.size() && 
			uri.substr(0, prefix.size()) == prefix) {
			return true;
		}
	}
	
	return false;
}

bool Router::isMethodAllowed(const Route& route, const std::string& method) const {
	if (route.methods.empty()) {
		return true; // No restriction
	}
	
	for (size_t i = 0; i < route.methods.size(); ++i) {
		if (route.methods[i] == method) {
			return true;
		}
	}
	
	return false;
}
