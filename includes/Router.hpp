#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "Handler.hpp"
#include <vector>
#include <string>

// Forward declaration
struct RouteConfig;

// Forward declaration
struct RouteConfig;

// Person 3: Request routing system
// URL pattern matching and method dispatch
class Router {
	public:
		Router();
		~Router();
		
		// Add a route
		// pattern: URL pattern (e.g., "/api/users", "/static/*")
		// methods: allowed HTTP methods (e.g., {"GET", "POST"})
		// handler: handler instance (Router takes ownership)
		void addRoute(const std::string& pattern, 
					  const std::vector<std::string>& methods,
					  Handler* handler);
		
		// Route a request to the appropriate handler
		// Returns response, or NULL if no route matches
		Response route(const Request& request);
		
		// Set default handler (for 404)
		void setDefaultHandler(Handler* handler);
		
		// Clear all routes
		void clear();
		
	private:
		struct Route {
			std::string pattern;
			std::vector<std::string> methods;
			Handler* handler;
		};
		
		std::vector<Route> routes_;
		Handler* defaultHandler_;
		
		// Pattern matching
		bool matchPattern(const std::string& pattern, const std::string& uri) const;
		bool isMethodAllowed(const Route& route, const std::string& method) const;
};
