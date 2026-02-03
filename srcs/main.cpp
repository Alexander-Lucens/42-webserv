#include <string>
#include <iostream>
#include "Colors.hpp"
#include "ConfigParser.hpp"

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
        
        for (size_t i = 0; i < servers.size(); i++) {
            const ServerConfig &server = servers[i];
            
            std::cout << "=== Server " << i << " ===" << std::endl;
            std::cout << "  Port: " << server.port << std::endl;
            std::cout << "  Host: " << server.host << std::endl;
            
            std::cout << "  Server names: ";
            for (size_t j = 0; j < server.server_names.size(); j++) {
                std::cout << server.server_names[j];
                if (j < server.server_names.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
            
            std::cout << "  Root: " << server.root << std::endl;
            
            std::cout << "  Index files: ";
            for (size_t j = 0; j < server.index.size(); j++) {
                std::cout << server.index[j];
                if (j < server.index.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
            
            std::cout << "  Client max body size: " << server.client_max_body_size << " bytes" << std::endl;
            
            if (!server.error_pages.empty()) {
                std::cout << "  Error pages:" << std::endl;
                for (std::map<int, std::string>::const_iterator it = server.error_pages.begin(); 
                     it != server.error_pages.end(); ++it) {
                    std::cout << "    " << it->first << " -> " << it->second << std::endl;
                }
            }
            
            if (!server.locations.empty()) {
                std::cout << "  Locations:" << std::endl;
                for (std::map<std::string, LocationConfig>::const_iterator it = server.locations.begin();
                     it != server.locations.end(); ++it) {
                    const LocationConfig &loc = it->second;
                    std::cout << "    Path: " << loc.path << std::endl;
                    if (!loc.root.empty())
                        std::cout << "      Root: " << loc.root << std::endl;
                    
                    if (!loc.index.empty()) {
                        std::cout << "      Index: ";
                        for (size_t j = 0; j < loc.index.size(); j++) {
                            std::cout << loc.index[j];
                            if (j < loc.index.size() - 1) std::cout << ", ";
                        }
                        std::cout << std::endl;
                    }
                    
                    if (!loc.methods.empty()) {
                        std::cout << "      Methods: ";
                        for (size_t j = 0; j < loc.methods.size(); j++) {
                            std::cout << loc.methods[j];
                            if (j < loc.methods.size() - 1) std::cout << ", ";
                        }
                        std::cout << std::endl;
                    }
                    
                    std::cout << "      Autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
                    
                    if (!loc.cgi_ext.empty())
                        std::cout << "      CGI: " << loc.cgi_ext << " -> " << loc.cgi_path << std::endl;
                }
            }
            
            std::cout << std::endl;
        }
        
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}