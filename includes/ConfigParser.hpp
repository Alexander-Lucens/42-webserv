#pragma once

#include "ConfigData.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

class ConfigParser {
    private:
        ConfigParser();
        ConfigParser(const ConfigParser &other);
        std::vector<ServerConfig>   _servers;

        LocationConfig  parse_location_block(std::ifstream &file, std::string path);
        std::string     trim(const std::string& str);
        void            remove_semicolon(std::string &str);
        void            parse_server_block(std::ifstream &file);


    public:
        ~ConfigParser();

        static ConfigParser &getInstance();
        std::vector<ServerConfig> parse(const std::string &config_path);
        const std::vector<ServerConfig> &getServers() const;
        const ServerConfig &getId(int order) const;
};