#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <set>
#include <string>
#include "Socket.hpp"
#include "ConfigParser.hpp"
#include "Logger.hpp"
#include "EventLoop.hpp"

class Server {
	private:
		std::vector<Socket*>		listeningSockets;
		std::vector<ServerConfig>	servers;
		std::set<int>				listeningPorts;
		std::string					inputCMD;


	public:
		Server(std::string configPath);
		~Server();

		void start();
};

#endif