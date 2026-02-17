#pragma once

#include <fcntl.h>
#include <map>
#include <vector>
#include <unistd.h>
#include "Colors.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class Connection;

class EventLoop {
	private:
		int _epoll_fd;
		std::vector<Socket*> _serverSockets;
		std::map<int, Connection*> _connections;

	public:
		/* ----- OCF ----- */
		// EventLoop();
		EventLoop(const std::vector<Socket*>& sockets);
		~EventLoop();
		EventLoop& operator=(const EventLoop &other);
		/* ------------- */

		// void addSocket(Socket& serverSocket);
		void init();
		void run();
};
