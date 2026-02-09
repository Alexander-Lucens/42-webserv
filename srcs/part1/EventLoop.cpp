#include "EventLoop.hpp"
#include <sys/epoll.h>

/* ----- OCF ----- */
// EventLoop::EventLoop() {
//     _epoll_fd = epoll_create1(0);
// 	if (_epoll_fd == -1) {
// 		throw std::runtime_error("Failed to create epoll file descriptor");
// 	}
// };

EventLoop::EventLoop(const std::vector<Socket>& sockets) : _serverSockets(sockets) {
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

// void EventLoop::addSocket(Socket& serverSocket) {
//     int fd = serverSocket.getFd();
//     struct epoll_event event;
//     event.events = EPOLLIN;
//     event.data.fd = fd;

//     if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
//         throw std::runtime_error("Failed to add server socket to epoll");
//     }

//     _serverSockets.push_back(serverSocket);
// }

void EventLoop::init() {
    for (size_t i = 0; i < _serverSockets.size(); i++) {
        int fd = _serverSockets[i].getFd();
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = fd;

        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
            throw std::runtime_error("EventLoop::init() -> Failed to add server socket to epoll");
        }
        std::cout << GREEN << " [EventLoop] Added Server Socket FD: " << fd << RESET << std::endl;
    }
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



void EventLoop::run() {
    const int MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];

    std::cout << "Server is running. Waiting..." << std::endl;

    while (true) {
        int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) throw std::runtime_error("epoll_wait failed");

        for (int i = 0; i < num_events; ++i) {
            int current_fd = events[i].data.fd;

            bool isServerSocket = false;
            int port = -1;
            for (size_t j = 0; j < _serverSockets.size(); ++j) {
                if (current_fd == _serverSockets[j].getFd()) {
                    isServerSocket = true;
                    port = _serverSockets[j].getPort();
                    break;
                }
            }

            if (isServerSocket) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(current_fd, (struct sockaddr *)&client_addr, &client_len);

                if (client_fd == -1) continue;

                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                _connections[client_fd] = new Connection(client_fd);
                _connections[client_fd]->set_request_port(port);

                struct epoll_event client_event;
                client_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                client_event.data.fd = client_fd;
                epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);

                std::cout << "New Client Connected! FD: " << client_fd << std::endl;
            } else {
                if (_connections.find(current_fd) != _connections.end()) {
                    Connection* conn = _connections[current_fd];

                    if (events[i].events & EPOLLIN) {
                        if (!conn->on_readable()) {
                            epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                            delete conn;
                            _connections.erase(current_fd);
                        }
                    }
                    if (events[i].events & EPOLLOUT) {
                        // TO DO
                        if (!conn->on_writable()) {
                            epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                            delete conn;
                            _connections.erase(current_fd);
							continue;
                        } // Implement logic to push data in connection back
                    }
                }
            }
        }
    }
}