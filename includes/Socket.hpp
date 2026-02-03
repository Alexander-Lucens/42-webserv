#pragma once

#include <netinet/in.h>
#include <stdexcept>

class Socket {
	private:
		int _fd;
		struct sockaddr_in _address;

	public:
		/* ----- OCF ----- */
		Socket();
		Socket(int existing_fd);
		~Socket();
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		/* ----------------- */

		void setup(int port);

		int getFd() const;
};