#include <iostream>
#include "Response.hpp"
#include "Colors.hpp"

bool testResponse() {
	Response res;
	res.version = "HTTP/1.1";
	res.setStatus(200, "OK");
	res.setHeader("Content-Type", "text/html");
	// res.setHeader("Content-Length", "13");
	res.body = "<h1>Hello from body</h1>";

	std::string expected = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 24\r\n\r\n<h1>Hello from body</h1>";
	std::string actual = res.toString();

	if (actual != expected) {
		ERROR("Response Test Failed!");
		std::cout << "Expected:\n" << expected << std::endl;
		std::cout << "Actual:\n" << actual << std::endl;
		return false;
	}

	SUCCESS("Response Test Passed!");
	return true;
}
