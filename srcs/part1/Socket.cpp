#include "Socket.hpp"

#include <unistd.h>
#include <fcntl.h>

#include <cstring>
#include <stdexcept>
#include <cerrno>

Socket::Socket() : _fd(-1), _port(0) {
	std::memset(&_address, 0, sizeof(_address));
}

Socket::~Socket() {
	if (_fd != -1) {
		close(_fd);
	}
}

Socket::Socket(int port) {
	_port = port;
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1) {
		throw std::runtime_error("Failed to create socket");
	}

	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		throw std::runtime_error("Failed to set socket options");
	}

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error("Failed to set socket to non-blocking");
	}

	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);

	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		throw std::runtime_error("Failed to bind socket");
	}

	if (listen(_fd, SOMAXCONN) < 0) {
		throw std::runtime_error("Failed to listen on socket");
	}
}

Socket::Socket(const Socket &other) : _fd(other._fd), _port(other._port) {
	std::memcpy(&_address, &other._address, sizeof(_address));

	const_cast<Socket&>(other)._fd = -1;
}

// Socket &Socket::operator=(const Socket &other) {
// 	if (this != &other) {
// 		if (_fd != -1) {
//             close(_fd);
//         }
// 		_fd = other._fd;
// 		_port = other._port;
// 		_address = other._address;
// 		const_cast<Socket&>(other)._fd = -1;
// 	}
// 	return *this;
// }

void Socket::setup(int port) {
	_port = port;
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1) {
		throw std::runtime_error("Failed to create socket");
	}

	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		throw std::runtime_error("Failed to set socket options");
	}

	if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error("Failed to set socket to non-blocking");
	}

	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);

	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		throw std::runtime_error("Failed to bind socket");
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