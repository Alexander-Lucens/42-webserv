#pragma once

#include <netinet/in.h>
#include <stdexcept>

class Socket {
	private:
		int _fd;
		int _port;
		struct sockaddr_in _address;

	public:
		/* ----- OCF ----- */
		Socket();
		Socket(int fd); // <- this one is for test, in project will be used Socket().setup(PORT);
		~Socket();
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		/* ----------------- */

		void setup(int port);

		int getFd() const;
		int getPort() const;
};