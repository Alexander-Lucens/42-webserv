#pragma once

#include <string>
#include <map>

class Response {
	public:
		Response();
		~Response();
		
		// Status
		int statusCode;
		std::string statusMessage;
		
		// Headers
		std::map<std::string, std::string> headers;
		
		// Body
		std::string body;
		
		// Helper methods
		void setStatus(int code, const std::string& message = "");
		void setHeader(const std::string& key, const std::string& value);
		std::string getHeader(const std::string& key) const;
		void clear();
		
		// Serialize to HTTP response string
		std::string toString() const;
		
		// Check if response is ready to send
		bool isReady() const;
};
