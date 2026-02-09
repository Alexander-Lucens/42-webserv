#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/epoll.h>


#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include "Colors.hpp"
#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "EventLoop.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return 1;
    }

    std::string configPath = argv[1];

    try {
        std::cout << YELLOW << "[Init] Parsing config: " << configPath << RESET << std::endl;

        ConfigParser &parser = ConfigParser::get_instance();
        const std::vector<ServerConfig> &servers = parser.parse(configPath);

        std::vector<Socket> listeningSockets;
        std::set<int> listeningPorts;

        for (size_t i = 0; i < servers.size(); ++i) {
            for (size_t j = 0; j < servers[i].ports.size(); ++j) {
                int port = servers[i].ports[j];
                if (listeningPorts.find(port) == listeningPorts.end()) {
                    Socket newSocket(port);
                    listeningSockets.push_back(newSocket);
                    listeningPorts.insert(port);
                    std::cout << "   -> Prepared Socket on port " << listeningSockets.back().getPort() << " fd: " << listeningSockets.back().getFd() << std::endl;
                }
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