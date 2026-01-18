#include <iostream>
#include "Socket.hpp"
#include "Colors.hpp"
#include <unistd.h>

bool testSocket() {
    try {
        Socket serverSocket;
        serverSocket.setup(8080);

        std::cout << BG_GREEN << BWHITE << " SERVER STARTED " << RESET << std::endl;
        std::cout << "Try to connect: " << BLUE << "curl http://localhost:8080" << RESET << std::endl;
        std::cout << "Waiting for connection..." << std::endl;

        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            int clientFd = accept(serverSocket.getFd(), (struct sockaddr*)&clientAddr, &clientLen);
            
            if (clientFd >= 0) {
                std::cout << "\n" << BG_MAGENTA << BWHITE << " NEW CONNECTION! " << RESET << std::endl;
                std::cout << GREEN << "Client FD: " << clientFd << RESET << std::endl;
                
                std::string msg = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
                write(clientFd, msg.c_str(), msg.length());
                close(clientFd);
                
                std::cout << "Greeting sent. Connection closed.\n" << std::endl;
            }
            
            usleep(1000);
        }

    } catch (const std::exception &e) {
        std::cerr << BG_RED << " ERROR " << RESET << " " << e.what() << std::endl;
		return false;
    }

    return true;
}
	