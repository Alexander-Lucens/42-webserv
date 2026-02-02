
#pragma once

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>

#include "Socket.hpp"
#include "Request.hpp"


class Connection {
	private:
		Socket					socket;
		std::string				read_buffer;
		std::string				write_buffer;
		int						current_read_index;
		Request					request;
		// std::vector<Request>	requests;

	public:
		Connection();
		Connection(int fd);
		~Connection();

		const std::string& get_read_buffer() const;

		void on_readable();

		void scan_buffer();
		bool parse_request_line();
		bool parse_request_headers();
		bool parse_request_body();

		void clean_buffer_for_new_request();
};
