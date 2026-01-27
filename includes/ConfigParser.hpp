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
        std::vector<ServerConfig>   _servers;

        void parseServerBlock(std::ifstream &file);
        LocationConfig parseLocationBlock(std::ifstream &file, std::string path);

        std::string trim(const std::string& str);
        void removeSemicolon(std::string &str);

    public:
        ConfigParser();
        ~ConfigParser();

        std::vector<ServerConfig> parse(const std::string &config_path);
};