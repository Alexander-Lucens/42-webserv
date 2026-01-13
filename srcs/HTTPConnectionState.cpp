#include "../includes/HTTPConnectionState.hpp"
#include "../includes/Connection.hpp"
#include "../includes/HTTPParser.hpp"
#include "../includes/HTTPSerializer.hpp"
#include "../includes/Buffer.hpp"

HTTPConnectionState::HTTPConnectionState(Connection* connection)
	: connection_(connection), requestComplete_(false), responseReady_(false),
	  keepAlive_(true), timeout_(60), parser_(NULL), serializer_(NULL) {
	parser_ = new HTTPParser();
	serializer_ = new HTTPSerializer();
}

HTTPConnectionState::~HTTPConnectionState() {
	if (parser_) {
		delete parser_;
	}
	if (serializer_) {
		delete serializer_;
	}
}

ssize_t HTTPConnectionState::onDataReceived(Buffer& buffer) {
	if (requestComplete_) {
		// Already have a complete request, ignore more data
		return 0;
	}
	
	ssize_t consumed = parser_->parse(buffer, currentRequest_);
	
	if (consumed < 0) {
		// Parse error
		currentResponse_.setStatus(400, "Bad Request");
		responseReady_ = true;
		requestComplete_ = true;
		return consumed;
	}
	
	if (parser_->isComplete()) {
		requestComplete_ = true;
		processRequest();
	}
	
	return consumed;
}

void HTTPConnectionState::processRequest() {
	// Check for keep-alive
	std::string connectionHeader = currentRequest_.getHeader("connection");
	if (connectionHeader == "close") {
		keepAlive_ = false;
	} else if (currentRequest_.httpVersion == "HTTP/1.1") {
		keepAlive_ = true; // HTTP/1.1 defaults to keep-alive
	} else {
		keepAlive_ = false; // HTTP/1.0 defaults to close
	}
}

void HTTPConnectionState::setResponse(const Response& response) {
	currentResponse_ = response;
	responseReady_ = true;
	prepareResponse();
}

void HTTPConnectionState::prepareResponse() {
	if (!responseReady_) {
		return;
	}
	
	// Set connection header based on keep-alive
	if (keepAlive_) {
		currentResponse_.setHeader("Connection", "keep-alive");
	} else {
		currentResponse_.setHeader("Connection", "close");
	}
	
	// Serialize response to connection's write buffer
	Buffer& writeBuffer = connection_->getWriteBuffer();
	serializer_->serialize(currentResponse_, writeBuffer);
}

ssize_t HTTPConnectionState::onReadyToWrite() {
	if (!responseReady_) {
		return 0;
	}
	
	// Response is already in write buffer, Person 1 will handle actual write
	return 0;
}

bool HTTPConnectionState::hasCompleteRequest() const {
	return requestComplete_;
}

Request& HTTPConnectionState::getRequest() {
	return currentRequest_;
}

const Request& HTTPConnectionState::getRequest() const {
	return currentRequest_;
}

bool HTTPConnectionState::hasResponseToSend() const {
	return responseReady_ && !connection_->getWriteBuffer().empty();
}

bool HTTPConnectionState::shouldKeepAlive() const {
	return keepAlive_;
}

void HTTPConnectionState::setKeepAlive(bool keepAlive) {
	keepAlive_ = keepAlive;
}

void HTTPConnectionState::resetForNextRequest() {
	if (!keepAlive_) {
		return;
	}
	
	currentRequest_.clear();
	parser_->reset();
	requestComplete_ = false;
	responseReady_ = false;
	currentResponse_.clear();
}

bool HTTPConnectionState::shouldClose() const {
	return !keepAlive_ || connection_->shouldClose();
}

int HTTPConnectionState::getTimeout() const {
	return timeout_;
}
