#include <cassert>
#include <iostream>
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"

void test_handle_get()
{
	Request request;
	request.method = "GET";
	request.path = "/";

	Response response = response.handle_get(request);
	std::string serialized = response.serialize();

	assert(serialized.find("HTTP/1.1 200 OK") != std::string::npos);
	assert(serialized.find("Server: webserv/1.0") != std::string::npos);
	std::cout << "✅ test_handle_get passed\n";
}


int main(void)
{
	try
	{
		test_handle_get();
		std::cout << "\nAll tests passed!\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return (1);
	}
	return (0);
}