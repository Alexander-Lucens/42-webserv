#include "Socket.hpp"
#include "Logger.hpp"

#include <unistd.h>
#include <fcntl.h>

#include <cstring>
#include <stdexcept>
#include <cerrno>
#include <sstream>

Socket::Socket() : _fd(-1), _port(0) {
	std::memset(&_address, 0, sizeof(_address));
}

Socket::~Socket() {
	if (_fd >= 0) {
		close(_fd);
	}
}

Socket::Socket(int port) {
	_port = port;
	_fd = socket(AF_INET6, SOCK_STREAM, 0);
	if (_fd == -1) {
		throw std::runtime_error("Failed to create socket");
	}
	
	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 ||
		setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		throw std::runtime_error("Failed to set socket options");
	}

	int no = 0;
	if (setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0) {
		throw std::runtime_error("Failed to set socket options for dual-stack");
	}

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error("Failed to set socket to non-blocking");
	}

	std::memset(&_address, 0, sizeof(_address));
	_address.sin6_family = AF_INET6;
	_address.sin6_addr = in6addr_any;
	_address.sin6_port = htons(port);

	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::stringstream ss;
		ss << "Failed to bind socket: " << strerror(errno);
		throw std::runtime_error(ss.str());
	}

	if (listen(_fd, SOMAXCONN) < 0) {
		throw std::runtime_error("Failed to listen on socket");
	}

	LOG_INFO("Socket created on port " << port << " with fd " << _fd);
}

Socket::Socket(const Socket &other) : _fd(other._fd), _port(other._port) {
	std::memcpy(&_address, &other._address, sizeof(_address));

	const_cast<Socket&>(other)._fd = -1;
}

void Socket::setup(int port) {
	_port = port;
	_fd = socket(AF_INET6, SOCK_STREAM, 0);
	if (_fd == -1) {
		throw std::runtime_error("Failed to create socket");
	}

	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		throw std::runtime_error("Failed to set socket options");
	}

	int no = 0;
	if (setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0) {
		throw std::runtime_error("Failed to set socket options for dual-stack");
	}

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error("Failed to set socket to non-blocking");
	}

	std::memset(&_address, 0, sizeof(_address));
	_address.sin6_family = AF_INET6;
	_address.sin6_addr = in6addr_any;
	_address.sin6_port = htons(port);

	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::stringstream ss;
		ss << "Failed to bind socket: " << strerror(errno);
		throw std::runtime_error(ss.str());
	}

	if (listen(_fd, SOMAXCONN) < 0) {
        throw std::runtime_error("Failed to listen on socket");
	}
}

int Socket::getFd() const {
	return _fd;
}

int Socket::getPort() const {
	return _port;
}