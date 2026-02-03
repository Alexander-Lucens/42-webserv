#pragma once

#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <unistd.h>
#include "Socket.hpp"
#include "Connection.hpp"

class EventLoop {
	private:
		int _epoll_fd;
		Socket& _serverSocket;
		std::map<int, Connection*> _connections;

	public:
		/* ----- OCF ----- */
		EventLoop(Socket& serverSocket);
		~EventLoop();
		EventLoop& operator=(const EventLoop &other);
		/* ------------- */

		void init();
		void run();
};
