#include <iostream>
#include "Request.hpp"
#include "Response.hpp"
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

int main() {

    bool requestTestResult = testRequest();
    bool responseTestResult = testResponse();

    if (requestTestResult && responseTestResult) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed." << std::endl;
    }

    return 0;
}
	