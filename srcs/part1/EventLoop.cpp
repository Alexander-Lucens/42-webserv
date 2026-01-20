#include "EventLoop.hpp"

/* ----- OCF ----- */
EventLoop::EventLoop() {
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}
}

EventLoop::~EventLoop() {
	for (std::map<int, Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		delete it->second;
	}
	close(_epoll_fd);
}

// just to avoid unexpected behavior
EventLoop& EventLoop::operator=(const EventLoop &other) {
	if (this != &other) {
		for (std::map<int, Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
			delete it->second;
		}
		_connections.clear();

		_epoll_fd = other._epoll_fd;
		for (std::map<int, Connection*>::const_iterator it = other._connections.begin(); it != other._connections.end(); ++it) {
			_connections[it->first] = new Connection(*(it->second));
		}
	}
	return *this;
}
/* ------------- */

void EventLoop::addSocket(const Socket &socket) {
	int fd = socket.getFd();
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = fd;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
		throw std::runtime_error("Failed to add file descriptor to epoll");
	}

	_connections[fd] = new Connection();
}

void EventLoop::removeSocket(int fd) {
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
		throw std::runtime_error("Failed to remove file descriptor from epoll");
	}

	std::map<int, Connection*>::iterator it = _connections.find(fd);
	if (it != _connections.end()) {
		delete it->second;
		_connections.erase(it);
	}
}

void EventLoop::run() {
	const int MAX_EVENTS = 10;
	struct epoll_event events[MAX_EVENTS];

	while (true) {
		int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1) {
			throw std::runtime_error("epoll_wait failed");
		}

		for (int i = 0; i < num_events; ++i) {
			int fd = events[i].data.fd;
			std::map<int, Connection*>::iterator it = _connections.find(fd);
			if (it != _connections.end()) {
				Connection* conn = it->second;
				// Handle the event for the connection (read/write)
			}
		}
	}
}

