#pragma once

#include "Buffer.hpp"
#include <string>

// Forward declaration for callback
class Request;
class Response;

// Person 1: Connection lifecycle management
// This represents a single client connection
class Connection {
	public:
		Connection(int fd, const std::string& clientHost, int clientPort);
		~Connection();
		
		// Connection info
		int getFd() const;
		const std::string& getClientHost() const;
		int getClientPort() const;
		
		// Buffer access
		Buffer& getReadBuffer();
		Buffer& getWriteBuffer();
		const Buffer& getReadBuffer() const;
		const Buffer& getWriteBuffer() const;
		
		// I/O operations (non-blocking)
		// Returns bytes read/written, or -1 on error
		ssize_t read();
		ssize_t write();
		
		// Check if connection should be closed
		bool shouldClose() const;
		void markForClose();
		
		// Check if there's data to write
		bool hasDataToWrite() const;
		
		// Connection state
		enum State {
			CONNECTED,
			READING,
			WRITING,
			CLOSING,
			CLOSED
		};
		State getState() const;
		void setState(State state);
		
		// User data pointer (for Person 2 to attach HTTP state)
		void* getUserData() const;
		void setUserData(void* data);
		
	private:
		int fd_;
		std::string clientHost_;
		int clientPort_;
		Buffer readBuffer_;
		Buffer writeBuffer_;
		State state_;
		bool shouldClose_;
		void* userData_; // For Person 2 to store HTTP connection state
};
