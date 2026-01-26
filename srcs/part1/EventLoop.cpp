#include "EventLoop.hpp"

// TMP implementation of Connection class just to make it work for now
Connection::Connection() : socket(), read_buffer(""), write_buffer(""), request() {}
Connection::Connection(int fd) : socket(fd), read_buffer(""), write_buffer(""), request() {}
Connection::~Connection() {}
Connection::Connection(const Connection &other) : socket(other.socket), read_buffer(other.read_buffer), write_buffer(other.write_buffer), request(other.request) {}
Connection& Connection::operator=(const Connection &other) {
	if (this != &other) {
		socket = other.socket;
		read_buffer = other.read_buffer;
		write_buffer = other.write_buffer;
		request = other.request;
	}
	return *this;
}
// -----------------------------

/* ----- OCF ----- */
EventLoop::EventLoop(Socket &serverSocket) : _serverSocket(serverSocket) {
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}
	init();
}

EventLoop::~EventLoop() {
	for (std::map<int, Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		delete it->second;
	}
	if (_epoll_fd != -1) close(_epoll_fd);
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

void EventLoop::init() {
    int fd = _serverSocket.getFd();
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        throw std::runtime_error("Failed to add server socket to epoll");
    }
}

void EventLoop::run() {
    const int MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];

    std::cout << "Server is running. Waiting..." << std::endl;

    while (true) {
        int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) throw std::runtime_error("epoll_wait failed");

        for (int i = 0; i < num_events; ++i) {
            int current_fd = events[i].data.fd;

            if (current_fd == _serverSocket.getFd()) {
                
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(current_fd, (struct sockaddr *)&client_addr, &client_len);
                
                if (client_fd == -1) continue;

                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                _connections[client_fd] = new Connection(client_fd);

                struct epoll_event client_event;
                client_event.events = EPOLLIN | EPOLLET;
                client_event.data.fd = client_fd;
                epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);

                std::cout << "New Client Connected! FD: " << client_fd << std::endl;
            }
            else {
                if (_connections.find(current_fd) != _connections.end()) {
                    Connection* conn = _connections[current_fd];
                    
                    // if (events[i].events & EPOLLIN) conn->handleRead();
                    // if (events[i].events & EPOLLOUT) conn->handleWrite();
                    
                    // TMP to avoid -Werror and show that its works
                    (void)conn;
					std::cout << "Connection details:"
						<< "\n -> FD: " << conn->socket.getFd() \
						<< "\n -> Read Buffer Size: " << conn->read_buffer.size() \
						<< "\n -> Write Buffer Size: " << conn->write_buffer.size() << std::endl;
                }
            }
        }
    }
}
