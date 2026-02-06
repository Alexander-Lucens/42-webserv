#pragma once

#include <netinet/in.h>

class Socket {
	private:
		int _fd;
		int _port;
		struct sockaddr_in _address;

		// Socket &operator=(const Socket &other);

	public:
		/* ----- OCF ----- */
		Socket();
		Socket(int port);
		Socket(const Socket &other); // <- this one is for test, in project will be used Socket().setup(PORT);

		~Socket();
		/* ----------------- */

		void setup(int port);
		int getFd() const;
		int getPort() const;
};