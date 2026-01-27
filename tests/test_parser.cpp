#include "ConfigParser.hpp"
#include <iostream>
#include "Colors.hpp"

// Oe line test run
// c++ -Wall -Wextra -Werror -std=c++98 -Iincludes srcs/ConfigParser.cpp srcs/test_parser.cpp -o test_parser

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./test_parser <config_file>" << std::endl;
        return 1;
    }

    try {
        ConfigParser parser;
        std::vector<ServerConfig> servers = parser.parse(argv[1]);

        std::cout << GREEN << "Successfully parsed " << servers.size() << " servers!" << RESET << std::endl;

        if (!servers.empty()) {
            ServerConfig &s = servers[0];
            std::cout << "Server 1:" << std::endl;
            std::cout << "  Port: " << s.port << std::endl;
            std::cout << "  Root: " << s.root << std::endl;
            std::cout << "  Locations count: " << s.locations.size() << std::endl;

            if (s.locations.find("/cgi-bin") != s.locations.end()) {
                std::cout << "  Found /cgi-bin!" << std::endl;
                std::cout << "    CGI Path: " << s.locations["/cgi-bin"].cgi_path << std::endl;
            }
        }

    } catch (const std::exception &e) {
        std::cerr << RED << e.what() << RESET << std::endl;
        return 1;
    }

    return 0;
}