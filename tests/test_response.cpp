#include <cassert>
#include <iostream>
#include "Response.hpp"
#include "Request.hpp"

void test_handle_get()
{
	Request request;
	request.method = "GET";
	request.path = "/";

	Response response;
	response = response.handle_get(request);
	std::string serialized = response.serialize();

	assert(serialized.find("HTTP/1.1 200 OK") != std::string::npos);
	assert(serialized.find("Server: webserv/1.0") != std::string::npos);
	std::cout << "✅ test_handle_get passed\n";
}

void test_post_upload_multipart()
{
	// Simulate multipart form data
	std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
	std::string body = 
		"------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
		"Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		"Hello World\r\n"
		"------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n";

		// Convert size_t to string using stringstream
	std::stringstream ss;
	ss << body.length();
	std::string content_length = ss.str();

	Request request;
	request.method = "POST";
	request.path = "/upload";
	request.body = body;
	request.setHeader("Content-Length", content_length);
	request.setHeader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");

	Response response;
	response = response.handle_post(request);
	std::string serialized = response.serialize();

	assert(serialized.find("HTTP/1.1 201 Created") != std::string::npos);
	assert(serialized.find("test.txt") != std::string::npos);
	assert(serialized.find("Location: /uploads/test.txt") != std::string::npos);

	std::cout << "✅ POST /upload test passed" << std::endl;
}


/* int main(void)
{
	try
	{
		test_handle_get();
		test_post_upload_multipart();
		std::cout << "\nAll tests passed!\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
	return 0;
 } */

