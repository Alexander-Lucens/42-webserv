#include "ConfigParser.hpp"
#include <iostream>


ConfigParser::ConfigParser() {}
ConfigParser::~ConfigParser() {}

ConfigParser &ConfigParser::get_instance() {
    static ConfigParser instance;
    return instance;
}

const std::vector<ServerConfig> &ConfigParser::get_servers() const {
    return _servers;
}

const ServerConfig &ConfigParser::get_config(int port, const std::string &host) const {
    const ServerConfig* defaultServer = NULL;
    for (size_t i = 0; i < _servers.size(); ++i) {
        const ServerConfig &curr = _servers[i];
        bool portMatch = false;
        for (size_t p = 0; p < curr.ports.size(); ++p) {
            if (curr.ports[p] == port) {
                portMatch = true;
                break;
            }
        }
        if (!portMatch) continue;

        if (defaultServer == NULL) {
            defaultServer = &curr;
        }
        for (size_t n = 0; n < curr.server_names.size(); ++n) {
            if (curr.server_names[n] == host) {
                return curr;
            }
        }
    }
    if (defaultServer) {
        return *defaultServer;
    }
    return _servers[0];
}

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
    _servers.clear();
    std::ifstream file(config_path.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open config file: " + config_path);
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

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
            std::string port;
            while (ss >> port) {
                remove_semicolon(port);
                server.ports.push_back(std::atoi(port.c_str())); // ? do we need to chack that or not but just for now let it be
            }
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
            std::string brace;
            ss >> path >> brace;
            if (brace != "{") {
                throw std::runtime_error("Error: Expected '{' after location path");
            }
            // Передаем остаток строки в parse_location_block
            LocationConfig loc = parse_location_block(file, path, ss);
            server.locations[path] = loc;
        }
        else {
            std::cout << "Warning: Unknown key in server block: " << key << std::endl;
        }
    }
    throw std::runtime_error("Error: Unexpected end of file inside server block");
}

LocationConfig ConfigParser::parse_location_block(std::ifstream &file, std::string path, std::stringstream &first_line_ss) {
    LocationConfig loc;
    loc.path = path;
    std::string line;

    std::string key;
    if (first_line_ss >> key) {
        if (key == "}") {
            return loc;
        }
        if (key == "root") {
            first_line_ss >> loc.root;
            remove_semicolon(loc.root);
        } else if (key == "index") {
            std::string idx;
            while (first_line_ss >> idx) { remove_semicolon(idx); loc.index.push_back(idx); }
        } else if (key == "methods") {
            std::string method;
            while (first_line_ss >> method) { remove_semicolon(method); loc.methods.push_back(method); }
        } else if (key == "autoindex") {
            std::string val; first_line_ss >> val; remove_semicolon(val); loc.autoindex = (val == "on");
        } else if (key == "cgi_pass") {
            first_line_ss >> loc.cgi_ext >> loc.cgi_path; remove_semicolon(loc.cgi_path);
        }
        std::string end_brace;
        if (first_line_ss.rdbuf()->in_avail() > 0) {
            if (first_line_ss >> end_brace && end_brace == "}") {
                return loc;
            }
        }
    }
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "}") {
            return loc;
        }

        std::stringstream ss(line);
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
// LocationConfig ConfigParser::parse_location_block(std::ifstream &file, std::string path) {
//     LocationConfig loc;
//     loc.path = path;
//     std::string line;

//     while (std::getline(file, line)) {
//         line = trim(line);
//         if (line.empty() || line[0] == '#') continue;

//         if (line == "}") {
//             return loc;
//         }

//         std::stringstream ss(line);
//         std::string key;
//         ss >> key;

//         if (key == "root") {
//             ss >> loc.root;
//             remove_semicolon(loc.root);
//         }
//         else if (key == "index") {
//             std::string idx;
//             while (ss >> idx) {
//                 remove_semicolon(idx);
//                 loc.index.push_back(idx);
//             }
//         }
//         else if (key == "methods") {
//             std::string method;
//             while (ss >> method) {
//                 remove_semicolon(method);
//                 loc.methods.push_back(method);
//             }
//         }
//         else if (key == "autoindex") {
//             std::string val;
//             ss >> val;
//             remove_semicolon(val);
//             loc.autoindex = (val == "on");
//         }
//         else if (key == "cgi_pass") {
//             ss >> loc.cgi_ext >> loc.cgi_path;
//             remove_semicolon(loc.cgi_path);
//         }
//     }
//     throw std::runtime_error("Error: Unexpected end of file inside location block");
// }