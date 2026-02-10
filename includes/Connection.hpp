
#pragma once

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>

#include "Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"


class Connection {
	private:
		int						_fd;
		std::string				read_buffer;
		std::string				write_buffer;
		int						current_read_index;
		Request					request;
		Response				response;

	public:
		// Connection();
		// Connection(Socket &socket);
		Connection(int fd);
		~Connection();

		const std::string& get_read_buffer() const;

		// New UPDATE
		bool on_writable();
		bool on_readable();
		///
		void clean_buffer_for_new_request();

		int  scan_buffer();
		int  parse_request_line();
		int  parse_request_headers();
		int  parse_request_body();
		void parse_uri();

		void set_request_port(int port) {
			request.port = port;
		}
};
