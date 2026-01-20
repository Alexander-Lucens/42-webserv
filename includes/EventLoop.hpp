#pragma once

#include <sys/epoll.h>
#include <map>
#include "Socket.hpp"
#include "Connection.hpp"

class EventLoop {
	private:
		int _epoll_fd;
		std::map<int, Connection*> _connections;

	public:
		/* ----- OCF ----- */
		EventLoop();
		~EventLoop();
		EventLoop& operator=(const EventLoop &other);
		/* ------------- */

		void addSocket(const Socket &socket);
		void removeSocket(int fd);
		void run();
};
