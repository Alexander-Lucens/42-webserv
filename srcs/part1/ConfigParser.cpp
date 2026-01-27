#include "ConfigParser.hpp"
#include <iostream>


ConfigParser::ConfigParser() {}
ConfigParser::~ConfigParser() {}

/* ----- Utils ----- */
std::string ConfigParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

void ConfigParser::remove_semicolon(std::string &str) {
    if (!str.empty() && str[str.size() - 1] == ';') {
        str.resize(str.size() - 1);
    }
}

/* ----- Logic ----- */

std::vector<ServerConfig> ConfigParser::parse(const std::string &config_path) {
    std::ifstream file(config_path.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open config file: " + config_path);
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "server {") {
            parse_server_block(file);
        } else {
            throw std::runtime_error("Error: Unknown directive outside server block: " + line);
        }
    }
    file.close();
    return _servers;
}

void ConfigParser::parse_server_block(std::ifstream &file) {
    ServerConfig server;
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "}") {
            _servers.push_back(server);
            return;
        }

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "listen") {
            ss >> server.port;
        }
        else if (key == "host") {
            ss >> server.host;
            remove_semicolon(server.host);
        }
        else if (key == "server_name") {
            std::string name;
            while (ss >> name) {
                remove_semicolon(name);
                server.server_names.push_back(name);
            }
        }
        else if (key == "root") {
            ss >> server.root;
            remove_semicolon(server.root);
        }
        else if (key == "index") {
            std::string idx;
            while (ss >> idx) {
                remove_semicolon(idx);
                server.index.push_back(idx);
            }
        }
        else if (key == "error_page") {
            int code;
            std::string path;
            ss >> code >> path;
            remove_semicolon(path);
            server.error_pages[code] = path;
        }
        else if (key == "client_max_body_size") {
            ss >> server.client_max_body_size;
            
        }
        else if (key == "location") {
            std::string path;
            ss >> path;
            
            LocationConfig loc = parse_location_block(file, path);
            server.locations[path] = loc;
        }
        else {
            std::cout << "Warning: Unknown key in server block: " << key << std::endl;
        }
    }
    throw std::runtime_error("Error: Unexpected end of file inside server block");
}

LocationConfig ConfigParser::parse_location_block(std::ifstream &file, std::string path) {
    LocationConfig loc;
    loc.path = path;
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "}") {
            return loc;
        }

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "root") {
            ss >> loc.root;
            remove_semicolon(loc.root);
        }
        else if (key == "index") {
            std::string idx;
            while (ss >> idx) {
                remove_semicolon(idx);
                loc.index.push_back(idx);
            }
        }
        else if (key == "methods") {
            std::string method;
            while (ss >> method) {
                remove_semicolon(method);
                loc.methods.push_back(method);
            }
        }
        else if (key == "autoindex") {
            std::string val;
            ss >> val;
            remove_semicolon(val);
            loc.autoindex = (val == "on");
        }
        else if (key == "cgi_pass") {
            ss >> loc.cgi_ext >> loc.cgi_path;
            remove_semicolon(loc.cgi_path);
        }
    }
    throw std::runtime_error("Error: Unexpected end of file inside location block");
}