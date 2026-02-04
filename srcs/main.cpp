#include <string>
#include <iostream>
#include "Colors.hpp"
#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "EventLoop.hpp"

/**
 * to test fast:
 * c++ -Wall -Wextra -Werror -std=c++98 -Iincludes srcs/part1/ConfigParser.cpp srcs/main.cpp  && ./a.out
 * 
 * Currently doing nothing but just for usage
 */
int main(int ac, char **av) {
    try {
        std::string config_path = (ac > 1) ? av[1] : "config.conf";

        ConfigParser::getInstance().parse(config_path);
        const std::vector<ServerConfig> &servers = ConfigParser::getInstance().getServers();
        std::cout << "Loaded " << servers.size() << " server(s)\n" << std::endl;

        EventLoop loop;
        for (size_t i = 0; i < servers.size(); i++) {
            const ServerConfig &server = servers[i];

            for (size_t j = 0; j < server.ports.size(); j++) {
                int cur_port = server.ports[j];
                Socket serverSocket;
                serverSocket.setup(cur_port);
                loop.addSocket(serverSocket);
                std::cout << "Element " << i << " --> server on PORT: " << cur_port << BLUE << " link http://" << server.server_names[0] << ":" << cur_port << RESET << std::endl;
            }
        }
        loop.run();
		
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}