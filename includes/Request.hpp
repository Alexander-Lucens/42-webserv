#pragma once

#include <string>
#include <map>

class Request {
	public:
		Request();
		~Request();
		
		// Request line
		std::string method;
		std::string uri;
		std::string httpVersion;
		
		// Headers (case-insensitive key lookup)
		std::map<std::string, std::string> headers;
		
		// Body
		std::string body;
		
		// Query string (parsed from URI)
		std::string queryString;
		
		// Helper methods
		std::string getHeader(const std::string& key) const;
		bool hasHeader(const std::string& key) const;
		void clear();
		
		// For parsing state
		enum ParseState {
			PARSING_REQUEST_LINE,
			PARSING_HEADERS,
			PARSING_BODY,
			PARSING_COMPLETE,
			PARSING_ERROR
		};
		ParseState parseState;
};
