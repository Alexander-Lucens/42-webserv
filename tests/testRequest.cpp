#include <iostream>
#include "Request.hpp"
#include "Colors.hpp"

bool testRequest() {
	Request req;
	req.method = "GET";
	req.path = "/index.html";
	req.version = "HTTP/1.1";
	req.setHeader("Host", "www.example.com");
	req.setHeader("User-Agent", "Mozilla/5.0");
	req.body = "";

	std::string expected = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0\r\n\r\n";
	std::string actual = req.toString();

	if (actual != expected) {
		ERROR("Request Test Failed!");
		std::cout << "Expected:\n" << expected << std::endl;
		std::cout << "Actual:\n" << actual << std::endl;
		return false;
	}
	SUCCESS("Request Test Passed!");
	return true;
}
