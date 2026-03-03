
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
		int						_port;
		std::string				read_buffer;
		std::string				write_buffer;

		Request					request;
		Response				response;

		int						error_code;
		size_t					MAX_REQUEST_SIZE;

	public:
		Connection(int fd, int port);
		~Connection();

		const std::string& get_read_buffer() const;

		bool on_writable();
		bool on_readable();

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
