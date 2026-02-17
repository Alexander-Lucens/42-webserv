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
#include <sstream>

#include "Colors.hpp"
#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"

int main(int argc, char **argv) {
    int status = 0;
    if (argc != 2) {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return 1;
    }
    std::string configPath = argv[1];
    
    Logger::init("webserv");
    std::vector<Socket*> listeningSockets;

    try {
        LOG_INFO("Starting web server...");
        LOG_INFO("Parsing config: " << configPath);

        ConfigParser &parser = ConfigParser::get_instance();
        std::vector<ServerConfig> servers = parser.parse(configPath);
        LOG_INFO("Configuration parsed successfully.");

        if (servers.empty()) {
            throw std::runtime_error("No servers defined in configuration.");
        }

        std::set<int> listeningPorts;

        for (size_t i = 0; i < servers.size(); ++i) {
            for (size_t j = 0; j < servers[i].ports.size(); ++j) {
                int port = servers[i].ports[j];
                if (listeningPorts.find(port) == listeningPorts.end()) {
                    Socket* newSocket = new Socket(port);
                    listeningSockets.push_back(newSocket);
                    listeningPorts.insert(port);
                    LOG_INFO("Prepared Socket on port " << listeningSockets.back()->getPort() 
                    << " fd: " << listeningSockets.back()->getFd());
                }
            }
        }

        if (listeningSockets.empty()) {
            throw std::runtime_error("No valid server ports found in config.");
        }

        LOG_INFO("Starting EventLoop...");
        EventLoop loop(listeningSockets);
        loop.run();

    } catch (const std::exception& e) {
        LOG_ERROR("Fatal crash: " << e.what());
        status = 1;
    }

    for (size_t i = 0; i < listeningSockets.size(); ++i) {
        delete listeningSockets[i];
    }

    return status;
}