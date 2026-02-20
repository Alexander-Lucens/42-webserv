#include "Server.hpp"
#include <csignal>

extern volatile sig_atomic_t g_running;

Server::Server(std::string configPath) {
	listeningSockets.clear();
	servers.clear();
	listeningPorts.clear();

	Logger::init("webserv");
	LOG_INFO("Starting web server...");
	LOG_INFO("Parsing config: " << configPath);
	ConfigParser &parser = ConfigParser::get_instance();
	servers = parser.parse(configPath);
	LOG_INFO("Configuration parsed successfully.");

	if (servers.empty()) {
		throw std::runtime_error("No servers defined in configuration.");
	}
}

void Server::start() {
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

	/**
	 * Could be implemented more comands like reload, status, drop-port, etc...
	 * but for now we will just handle exit command to gracefully shutdown the server
	 */
	inputCMD;
	if (std::cin >> inputCMD && inputCMD == "exit") {
		g_running = 0;
		LOG_INFO("Exit command received. Shutting down...");
	}

	if (!g_running) {
		LOG_INFO("Shutting down gracefully...");
	}
}

Server::~Server() {
	for (size_t i = 0; i < listeningSockets.size(); ++i) {
		delete listeningSockets[i];
	}
}