#pragma once

#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <unistd.h>
#include "Socket.hpp"
#include "Connection.hpp"

class Connection;

class EventLoop {
	private:
		int _epoll_fd;
		std::vector<Socket> _serverSockets;
		std::map<int, Connection*> _connections;

	public:
		/* ----- OCF ----- */
		EventLoop();
		// EventLoop(Socket& serverSocket);
		~EventLoop();
		EventLoop& operator=(const EventLoop &other);
		/* ------------- */

		void addSocket(Socket& serverSocket);
		// void init();
		void run();
};
