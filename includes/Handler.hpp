#pragma once

#include "Request.hpp"
#include "Response.hpp"

// Person 3: Handler interface
// All request handlers must implement this interface
class Handler {
	public:
		virtual ~Handler() {}
		
		// Process a request and generate a response
		// This is the main interface: Request -> Response
		virtual Response handle(const Request& request) = 0;
		
		// Check if this handler can handle the given request
		virtual bool canHandle(const Request& request) const = 0;
};
