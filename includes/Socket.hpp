#pragma once

#include <string>

// Person 1: Socket operations
class Socket {
	public:
		Socket();
		~Socket();
		
		// Create and bind socket
		bool create();
		bool bind(const std::string& host, int port);
		bool listen(int backlog = 128);
		
		// Accept new connection
		int accept(std::string& clientHost, int& clientPort);
		
		// Socket options
		bool setNonBlocking(bool nonBlocking = true);
		bool setReuseAddr(bool reuse = true);
		
		// Get socket file descriptor
		int getFd() const;
		
		// Close socket
		void close();
		
		// Check if socket is valid
		bool isValid() const;
		
	private:
		int fd_;
		bool isNonBlocking_;
};
