
#pragma once

#include <string>
#include <map>
#include <iostream>
#include <sstream>

#include "Request.hpp"


class Connection {
	public:
		std::string socket;
		std::string read_buffer;
		std::string write_buffer;
		Request		request;

		Connection();
		~Connection();
		Connection(const Connection &other);
		Connection& operator=(const Connection &other);
};


// void client_connection (Connection client&, char* data&, int size);