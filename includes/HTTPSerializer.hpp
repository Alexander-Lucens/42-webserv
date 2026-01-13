#pragma once

#include "Response.hpp"
#include "Buffer.hpp"

// Person 2: HTTP response serializer
// Converts Response objects to HTTP wire format
class HTTPSerializer {
	public:
		HTTPSerializer();
		~HTTPSerializer();
		
		// Serialize response to buffer
		// Returns true on success
		bool serialize(const Response& response, Buffer& buffer);
		
		// Check if serialization is needed
		bool needsSerialization(const Response& response) const;
		
	private:
		// Helper to format headers
		void addHeader(Buffer& buffer, const std::string& key, const std::string& value);
};
