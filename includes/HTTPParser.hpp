#pragma once

#include "Request.hpp"
#include "Buffer.hpp"

// Person 2: HTTP request parser
// Handles partial reads and validates HTTP format
class HTTPParser {
	public:
		HTTPParser();
		~HTTPParser();
		
		// Parse HTTP request from buffer
		// Returns number of bytes consumed, or -1 on error
		// This is the callback interface for Person 1
		ssize_t parse(Buffer& buffer, Request& request);
		
		// Check if parsing is complete
		bool isComplete() const;
		
		// Reset parser state
		void reset();
		
		// Get last error message
		std::string getError() const;
		
	private:
		// Internal parsing methods
		bool parseRequestLine(const std::string& line, Request& request);
		bool parseHeaderLine(const std::string& line, Request& request);
		bool parseBody(Buffer& buffer, Request& request);
		
		// Helper methods
		std::string readLine(Buffer& buffer);
		bool isValidMethod(const std::string& method) const;
		bool isValidHTTPVersion(const std::string& version) const;
		
		// State
		Request::ParseState currentState_;
		size_t expectedBodyLength_;
		std::string errorMessage_;
};
