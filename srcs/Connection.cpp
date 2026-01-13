#include "../includes/Connection.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

Connection::Connection(int fd, const std::string& clientHost, int clientPort)
	: fd_(fd), clientHost_(clientHost), clientPort_(clientPort),
	  state_(CONNECTED), shouldClose_(false), userData_(NULL) {
}

Connection::~Connection() {
	if (fd_ >= 0) {
		::close(fd_);
	}
}

int Connection::getFd() const {
	return fd_;
}

const std::string& Connection::getClientHost() const {
	return clientHost_;
}

int Connection::getClientPort() const {
	return clientPort_;
}

Buffer& Connection::getReadBuffer() {
	return readBuffer_;
}

Buffer& Connection::getWriteBuffer() {
	return writeBuffer_;
}

const Buffer& Connection::getReadBuffer() const {
	return readBuffer_;
}

const Buffer& Connection::getWriteBuffer() const {
	return writeBuffer_;
}

ssize_t Connection::read() {
	if (fd_ < 0) {
		return -1;
	}
	
	char buffer[4096];
	ssize_t bytesRead = ::read(fd_, buffer, sizeof(buffer));
	
	if (bytesRead > 0) {
		readBuffer_.append(buffer, bytesRead);
	} else if (bytesRead == 0) {
		// EOF - connection closed by client
		markForClose();
	}
	
	return bytesRead;
}

ssize_t Connection::write() {
	if (fd_ < 0 || writeBuffer_.empty()) {
		return 0;
	}
	
	const char* data = writeBuffer_.data();
	size_t dataSize = writeBuffer_.size();
	
	ssize_t bytesWritten = ::write(fd_, data, dataSize);
	
	if (bytesWritten > 0) {
		writeBuffer_.readString(bytesWritten); // Remove written data from buffer
	}
	
	return bytesWritten;
}

bool Connection::shouldClose() const {
	return shouldClose_;
}

void Connection::markForClose() {
	shouldClose_ = true;
	state_ = CLOSING;
}

bool Connection::hasDataToWrite() const {
	return !writeBuffer_.empty();
}

Connection::State Connection::getState() const {
	return state_;
}

void Connection::setState(State state) {
	state_ = state;
}

void* Connection::getUserData() const {
	return userData_;
}

void Connection::setUserData(void* data) {
	userData_ = data;
}
