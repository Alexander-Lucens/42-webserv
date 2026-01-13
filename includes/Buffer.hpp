#pragma once

#include <string>
#include <vector>

// Person 1: Buffer management for reading/writing
class Buffer {
	public:
		Buffer();
		~Buffer();
		
		// Append data to buffer
		void append(const char* data, size_t len);
		void append(const std::string& data);
		
		// Read data from buffer (removes from buffer)
		size_t read(char* dest, size_t maxLen);
		std::string readString(size_t maxLen);
		
		// Peek at data without removing
		size_t peek(char* dest, size_t maxLen, size_t offset = 0) const;
		
		// Get buffer info
		size_t size() const;
		bool empty() const;
		void clear();
		
		// Find operations
		size_t find(const std::string& pattern, size_t startPos = 0) const;
		
		// Get raw pointer (for direct access)
		const char* data() const;
		
	private:
		std::vector<char> buffer_;
};
