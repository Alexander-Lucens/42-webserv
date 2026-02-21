#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>

class Socket {
	private:
		int _fd;
		int _port;
		struct sockaddr_in6 _address;

	public:
		/* ----- OCF ----- */
		Socket();
		Socket(int port);
		Socket(const Socket &other);

		~Socket();
		/* ----------------- */

		void setup(int port);
		int getFd() const;
		int getPort() const;
};