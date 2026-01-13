#include "../includes/Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

Socket::Socket() : fd_(-1), isNonBlocking_(false) {
}

Socket::~Socket() {
	close();
}

bool Socket::create() {
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0) {
		return false;
	}
	return true;
}

bool Socket::setReuseAddr(bool reuse) {
	if (fd_ < 0) {
		return false;
	}
	int opt = reuse ? 1 : 0;
	return setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0;
}

bool Socket::setNonBlocking(bool nonBlocking) {
	if (fd_ < 0) {
		return false;
	}
	int flags = fcntl(fd_, F_GETFL, 0);
	if (flags < 0) {
		return false;
	}
	if (nonBlocking) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~O_NONBLOCK;
	}
	if (fcntl(fd_, F_SETFL, flags) < 0) {
		return false;
	}
	isNonBlocking_ = nonBlocking;
	return true;
}

bool Socket::bind(const std::string& host, int port) {
	if (fd_ < 0) {
		return false;
	}
	
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	
	if (host.empty() || host == "0.0.0.0") {
		addr.sin_addr.s_addr = INADDR_ANY;
	} else {
		if (inet_aton(host.c_str(), &addr.sin_addr) == 0) {
			return false;
		}
	}
	
	return ::bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

bool Socket::listen(int backlog) {
	if (fd_ < 0) {
		return false;
	}
	return ::listen(fd_, backlog) == 0;
}

int Socket::accept(std::string& clientHost, int& clientPort) {
	if (fd_ < 0) {
		return -1;
	}
	
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientFd = ::accept(fd_, (struct sockaddr*)&clientAddr, &clientLen);
	
	if (clientFd >= 0) {
		clientHost = inet_ntoa(clientAddr.sin_addr);
		clientPort = ntohs(clientAddr.sin_port);
	}
	
	return clientFd;
}

int Socket::getFd() const {
	return fd_;
}

void Socket::close() {
	if (fd_ >= 0) {
		::close(fd_);
		fd_ = -1;
	}
}

bool Socket::isValid() const {
	return fd_ >= 0;
}
