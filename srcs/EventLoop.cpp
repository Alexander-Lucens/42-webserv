#include "EventLoop.hpp"
#include <sys/epoll.h>
#include <csignal>

extern volatile sig_atomic_t g_running;

void graceful_close(int fd) {
    try {
        close(fd);
    } catch (...) {}
}

EventLoop::EventLoop(const std::vector<Socket*>& sockets) : _serverSockets(sockets) {
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1) {
		throw std::runtime_error("Failed to create epoll file descriptor");
	}
	init();
}

EventLoop::~EventLoop() {
	for (std::map<int, Connection*>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		graceful_close(it->first);
        try {
            delete it->second;
        } catch (...) {}
	}
	if (_epoll_fd != -1) {
        graceful_close(_epoll_fd);
    }
    for (size_t i = 0; i < _serverSockets.size(); ++i) {
        graceful_close(_serverSockets[i]->getFd());
    }
}

void EventLoop::init() {
    for (size_t i = 0; i < _serverSockets.size(); i++) {
        int fd = _serverSockets[i]->getFd();
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = fd;

        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
            throw std::runtime_error("EventLoop::init() -> Failed to add server socket to epoll");
        }
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

    while (g_running) {
        int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, 500);        
        if (num_events == -1) {
            continue;
        }

        if (!g_running) {
            std::cout << "Shutdown signal received" << std::endl;
            break;
        }

        for (int i = 0; i < num_events; ++i) {
            int current_fd = events[i].data.fd;

            bool isServerSocket = false;
            int port = -1;
            for (size_t j = 0; j < _serverSockets.size(); ++j) {
                if (current_fd == _serverSockets[j]->getFd()) {
                    isServerSocket = true;
                    port = _serverSockets[j]->getPort();
                    break;
                }
            }

            if (isServerSocket) {
                while (true) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(current_fd, (struct sockaddr *)&client_addr, &client_len);

                    if (client_fd == -1) {
                        break;
                    }

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);

                    _connections[client_fd] = new Connection(client_fd, port);
                    
                    struct epoll_event client_event;
                    client_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    client_event.data.fd = client_fd;
                    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);

                    std::cout << "New Client Connected! FD: " << client_fd << std::endl;
                }
            } else {
                if (_connections.find(current_fd) != _connections.end()) {
                    Connection* conn = _connections[current_fd];

                    if (events[i].events & EPOLLIN) {
                        if (!conn->on_readable()) {
                            epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                            delete conn;
                            _connections.erase(current_fd);
                            close(current_fd);
                            continue;
                        }
                    }
                    if (events[i].events & EPOLLOUT) {
                        if (!conn->on_writable()) {
                            epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                            delete conn;
                            _connections.erase(current_fd);
                            close(current_fd);
                            continue;
                        }
                    }
                }
            }
        }
    }

    LOG_INFO("Closing all remaining connections...");
    std::map<int, Connection*>::iterator it = _connections.begin();
    while (it != _connections.end()) {
        int fd = it->first;
        Connection* conn = it->second;
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        delete conn;
        close(fd);
        _connections.erase(it++);
    }
}