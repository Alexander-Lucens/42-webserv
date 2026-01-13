#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "Buffer.hpp"
#include <string>

// Forward declaration
class Connection;

// Person 2: HTTP connection state machine
// Manages keep-alive, connection lifecycle, and integrates with Person 1's I/O
class HTTPConnectionState {
	public:
		HTTPConnectionState(Connection* connection);
		~HTTPConnectionState();
		
		// Process incoming data (called by Person 1 when bytes arrive)
		// Returns number of bytes consumed, or -1 on error
		ssize_t onDataReceived(Buffer& buffer);
		
		// Set response to send (called by Person 3)
		void setResponse(const Response& response);
		
		// Process outgoing data (called by Person 1 when ready to write)
		ssize_t onReadyToWrite();
		
		// Check if request is complete and ready for Person 3
		bool hasCompleteRequest() const;
		Request& getRequest();
		const Request& getRequest() const;
		
		// Check if response is ready to send
		bool hasResponseToSend() const;
		
		// Connection management
		bool shouldKeepAlive() const;
		void setKeepAlive(bool keepAlive);
		
		// Reset state for next request (if keep-alive)
		void resetForNextRequest();
		
		// Check if connection should be closed
		bool shouldClose() const;
		
		// Get timeout value for this connection
		int getTimeout() const;
		
	private:
		Connection* connection_;
		Request currentRequest_;
		Response currentResponse_;
		bool requestComplete_;
		bool responseReady_;
		bool keepAlive_;
		int timeout_;
		
		// Parser instance
		class HTTPParser* parser_;
		class HTTPSerializer* serializer_;
		
		// Internal methods
		void processRequest();
		void prepareResponse();
};
