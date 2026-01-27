#include <iostream>
#include "Socket.hpp"
#include "Colors.hpp"
#include <unistd.h>

#include "Response.hpp"

bool testSocket() {
    try {
        Socket serverSocket;
		Request request;
	
        serverSocket.setup(8080);

        std::cout << BG_GREEN << BWHITE << " SERVER STARTED " << RESET << std::endl;
        std::cout << "Try to connect: " << BLUE << "curl http://localhost:8080" << RESET << std::endl;
        std::cout << "Waiting for connection..." << std::endl;

        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            int clientFd = accept(serverSocket.getFd(), (struct sockaddr*)&clientAddr, &clientLen);
            
            if (clientFd >= 0) {
                std::cout << "\"" << BG_MAGENTA << BWHITE << " NEW CONNECTION! " << RESET << std::endl;
                std::cout << GREEN << "Client FD: " << clientFd << RESET << std::endl;
                
                // Read HTTP request from socket
                char buffer[4096] = {0};
                ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
                
                if (bytesRead > 0) {
					std::string rawRequest(buffer);
					std::cout << "Raw Request:\n" << rawRequest << std::endl;
					
					// Parse request line
					std::istringstream iss(rawRequest);
					std::string method, path, httpVersion;
					iss >> method >> path >> httpVersion;
					
					Request request;
					request.method = method;
					request.path = path;
					
					// Parse headers and body
					std::string line;
					std::getline(iss, line);  // consume rest of request line
					
					std::map<std::string, std::string> headers;
					while (std::getline(iss, line)) {
						if (line == "\r" || line.empty())
							break;  // Empty line marks end of headers
						
						size_t colon_pos = line.find(":");
						if (colon_pos != std::string::npos) {
							std::string key = line.substr(0, colon_pos);
							std::string value = line.substr(colon_pos + 2);
							// Remove trailing \r
							if (!value.empty() && value[value.length() - 1] == '\r')
								value.erase(value.length() - 1);
							request.setHeader(key, value);
						}
					}
					
					// Read body (if Content-Length header exists)
					std::string content_length_str = request.getHeader("Content-Length");
					if (!content_length_str.empty()) {
						int content_length = std::atoi(content_length_str.c_str());
						std::string body;
						body.resize(content_length);
						iss.read(&body[0], content_length);
						request.body = body;
					}
					
					Response response = response.handle_request(request);
					
					
					std::string msg = response.serialize();
					std::cout << msg << std::endl;
					write(clientFd, msg.c_str(), msg.length());
				}
								
                close(clientFd);
                std::cout << "Connection closed.\n" << std::endl;
            }
            
            usleep(10000);
        }

    } catch (const std::exception &e) {
        std::cerr << BG_RED << " ERROR " << RESET << " " << e.what() << std::endl;
		return false;
    }

    return true;
}
	