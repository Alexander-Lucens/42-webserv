#include <iostream>
#include "Socket.hpp"
#include "EventLoop.hpp"
#include "Colors.hpp"

int main() {
    try {
        std::cout << YELLOW << "[Init] 1. Creating Server Socket..." << RESET << std::endl;
        Socket serverSocket;
        serverSocket.setup(8080);
        std::cout << GREEN << "   -> Success! Listening on port 8080 (FD: " << serverSocket.getFd() << ")" << RESET << std::endl;

        std::cout << YELLOW << "[Init] 2. Initializing EventLoop..." << RESET << std::endl;
        EventLoop loop(serverSocket);
        std::cout << GREEN << "   -> Success! EventLoop created & Server Socket added to epoll." << RESET << std::endl;

        std::cout << "\n" << BG_BLUE << BWHITE << " SERVER IS RUNNING " << RESET << std::endl;
        std::cout << "Test commands:" << std::endl;
        std::cout << "  1. Browser:   " << BLUE << "http://localhost:8080" << RESET << std::endl;
        std::cout << "  2. Terminal:  " << BLUE << "nc localhost 8080" << RESET << " (type 'Hi' and press Enter)" << std::endl;
        std::cout << "Logs:" << std::endl;

        loop.run();

    } catch (const std::exception& e) {
        std::cerr << BG_RED << " CRITICAL ERROR " << RESET << " " << e.what() << std::endl;
        return 1;
    }

    return 0;
}