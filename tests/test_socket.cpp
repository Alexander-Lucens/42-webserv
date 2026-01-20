#include <iostream>
#include "Socket.hpp"
#include "Colors.hpp"
#include <unistd.h>

#include "Response.hpp"

bool testSocket() {
    try {
        Socket serverSocket;
		Response response;
		response.set_body("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>403 Error</title><link rel=\"stylesheet\" href=\"../base_page/style.css\"> \
	</head><body><h1 class=\"header-error\">403</h1><p class=\"body-error\">Forbidden. You're missing permissions. </p></body></body></html>");
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
                
                
				std::string msg = response.serialize();
				std::cout << msg << std::endl;
                write(clientFd, msg.c_str(), msg.length());
                close(clientFd);
                
                std::cout << "Greeting sent. Connection closed.\n" << std::endl;
            }
            
            usleep(10000);
        }

    } catch (const std::exception &e) {
        std::cerr << BG_RED << " ERROR " << RESET << " " << e.what() << std::endl;
		return false;
    }

    return true;
}
	