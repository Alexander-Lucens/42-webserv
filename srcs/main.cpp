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
#include <sstream>

#include "Server.hpp"

#include <csignal>
#include <sys/wait.h>

volatile std::sig_atomic_t g_running = 1;

void handle_signal(int sig) {
    (void)sig;
    g_running = 0;
    static int signal_count = 0;
    signal_count++;
    if (signal_count > 1) {
        std::cerr << "Force exit..." << std::endl;
        _exit(EXIT_SUCCESS);
    }
}

void handle_sigchld(int sig) {
    (void)sig;
    int status;
    while(waitpid(-1, &status, WNOHANG) > 0);
}

int main(int argc, char **argv) {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);
    std::signal(SIGCHLD, handle_sigchld);

    int status = 0;
    if (argc != 2) {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return 1;
    }
    std::string configPath = argv[1];
    try {
        Server server(configPath);
        server.start();
    } catch (const std::exception& e) {
        LOG_ERROR(e.what());
        status = 1;
    }
    return status;
}
