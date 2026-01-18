#include <cassert>
#include <iostream>
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"

void test_handle_get()
{
	Request request;
	request.method = "GET";

	Response response;
	response = response.handle_get(request);

	assert(response.status_code == 200);
	assert(response.headers["Server"] == "webserv/1.0");
	assert(response.html_body.find("GET Request Successful") != std::string::npos);
	std::cout << "✅ test_handle_get passed\n";
	}

void test_serialize()
{
	Request request;
	request.method = "GET";

	Response response;
	response = response.handle_get(request);

	std::string serialized = response.serialize(response);

	assert(serialized.find("HTTP/1.1 200 OK") != std::string::npos);
	assert(serialized.find("Content-Length:") != std::string::npos);
	assert(serialized.find("GET Request Successful") != std::string::npos);
	std::cout << "✅ test_serialize passed\n";
}

int main(void)
{
	try
	{
		test_handle_get();
		test_serialize();
		std::cout << "\nAll tests passed!\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return (1);
	}
	return (0);
}