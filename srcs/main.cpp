#include <iostream>
#include <vector>
#include <set>
#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "EventLoop.hpp"
#include "Colors.hpp"

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return 1;
    }

    std::string configPath = (argc == 2) ? argv[1] : "config.conf";

    try {
        std::cout << YELLOW << "[Init] Parsing config: " << configPath << RESET << std::endl;
        ConfigParser parser;
        std::vector<ServerConfig> configs = parser.parse(configPath);

        std::vector<Socket> listeningSockets;
        std::set<int> portsBound;

        for (size_t i = 0; i < configs.size(); ++i) {
            int port = configs[i].port;
            
            if (portsBound.find(port) == portsBound.end()) {
                Socket sock;
                sock.setup(port);
                listeningSockets.push_back(sock);
                portsBound.insert(port);
                std::cout << GREEN << "   -> Prepared Socket on port " << port << RESET << std::endl;
            }
        }

        if (listeningSockets.empty()) {
            throw std::runtime_error("No valid server ports found in config.");
        }

        std::cout << YELLOW << "[Init] Starting EventLoop..." << RESET << std::endl;
        EventLoop loop(listeningSockets);
        loop.run();

    } catch (const std::exception& e) {
        std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
        return 1;
    }

    return 0;
}