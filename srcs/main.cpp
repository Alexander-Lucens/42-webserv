#include "../includes/Socket.hpp"
#include "../includes/EventLoop.hpp"
#include "../includes/Connection.hpp"
#include "../includes/HTTPConnectionState.hpp"
#include "../includes/Router.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/StaticFileHandler.hpp"
#include "../includes/Handler.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

// Integration layer: Ties Person 1, Person 2, and Person 3 together
class WebServer {
	public:
		WebServer() : eventLoop_(), config_() {
		}
		
		~WebServer() {
			// Cleanup connections
			for (std::map<int, Connection*>::iterator it = connections_.begin();
				 it != connections_.end(); ++it) {
				delete it->second;
			}
			connections_.clear();
			
			// Cleanup HTTP states
			for (std::map<int, HTTPConnectionState*>::iterator it = httpStates_.begin();
				 it != httpStates_.end(); ++it) {
				delete it->second;
			}
			httpStates_.clear();
			
			// Cleanup sockets
			for (size_t i = 0; i < sockets_.size(); ++i) {
				delete sockets_[i];
			}
			sockets_.clear();
		}
		
		bool init(const std::string& configFile) {
			// Load configuration (Person 3)
			if (!configFile.empty()) {
				if (!config_.loadFromFile(configFile)) {
					std::cerr << "Failed to load config file, using defaults" << std::endl;
					config_.loadDefault();
				}
			} else {
				config_.loadDefault();
			}
			
			// Initialize event loop (Person 1)
			if (!eventLoop_.init()) {
				std::cerr << "Failed to initialize event loop" << std::endl;
				return false;
			}
			
			// Create sockets for each server block (Person 1)
			const std::vector<ServerBlock>& servers = config_.getServerBlocks();
			for (size_t i = 0; i < servers.size(); ++i) {
				Socket* socket = new Socket();
				if (!socket->create()) {
					std::cerr << "Failed to create socket" << std::endl;
					delete socket;
					continue;
				}
				
				socket->setReuseAddr(true);
				socket->setNonBlocking(true);
				
				if (!socket->bind(servers[i].host, servers[i].port)) {
					std::cerr << "Failed to bind to " << servers[i].host 
							  << ":" << servers[i].port << std::endl;
					delete socket;
					continue;
				}
				
				if (!socket->listen(128)) {
					std::cerr << "Failed to listen" << std::endl;
					delete socket;
					continue;
				}
				
				sockets_.push_back(socket);
				eventLoop_.addFd(socket->getFd(), true, false);
				
				std::cout << "Server listening on " << servers[i].host 
						  << ":" << servers[i].port << std::endl;
			}
			
			// Setup router (Person 3)
			setupRouter();
			
			return true;
		}
		
		void run() {
			std::cout << "WebServer running..." << std::endl;
			
			while (true) {
				// Poll for events (Person 1)
				int numEvents = eventLoop_.poll(1000); // 1 second timeout
				
				if (numEvents < 0) {
					std::cerr << "Poll error" << std::endl;
					break;
				}
				
				if (numEvents == 0) {
					continue; // Timeout
				}
				
				// Process events
				processEvents();
			}
		}
		
	private:
		EventLoop eventLoop_;
		ServerConfig config_;
		std::vector<Socket*> sockets_;
		std::map<int, Connection*> connections_;
		std::map<int, HTTPConnectionState*> httpStates_;
		std::map<int, Router*> routers_; // Router per server socket
		Router defaultRouter_;
		
		void setupRouter() {
			// Setup default router with static file handler
			StaticFileHandler* handler = new StaticFileHandler("./www", "index.html", false);
			std::vector<std::string> methods;
			methods.push_back("GET");
			methods.push_back("HEAD");
			defaultRouter_.addRoute("/", methods, handler);
		}
		
		void processEvents() {
			// Check listening sockets for new connections
			for (size_t i = 0; i < sockets_.size(); ++i) {
				int listenFd = sockets_[i]->getFd();
				if (eventLoop_.isReadReady(listenFd)) {
					handleNewConnection(listenFd);
				}
			}
			
			// Process existing connections
			std::vector<int> toRemove;
			for (std::map<int, Connection*>::iterator it = connections_.begin();
				 it != connections_.end(); ++it) {
				int fd = it->first;
				Connection* conn = it->second;
				
				// Check for read events
				if (eventLoop_.isReadReady(fd)) {
					handleRead(fd, conn);
				}
				
				// Check for write events
				if (eventLoop_.isWriteReady(fd)) {
					handleWrite(fd, conn);
				}
				
				// Check if connection should be closed
				if (conn->shouldClose() || 
					(httpStates_[fd] && httpStates_[fd]->shouldClose())) {
					toRemove.push_back(fd);
				}
			}
			
			// Remove closed connections
			for (size_t i = 0; i < toRemove.size(); ++i) {
				removeConnection(toRemove[i]);
			}
		}
		
		void handleNewConnection(int listenFd) {
			// Find the socket that matches this listenFd
			Socket* listenSocket = NULL;
			for (size_t i = 0; i < sockets_.size(); ++i) {
				if (sockets_[i]->getFd() == listenFd) {
					listenSocket = sockets_[i];
					break;
				}
			}
			if (!listenSocket) {
				return;
			}
			
			std::string clientHost;
			int clientPort;
			int clientFd = listenSocket->accept(clientHost, clientPort);
			
			if (clientFd < 0) {
				return;
			}
			
			// Create connection (Person 1)
			Connection* conn = new Connection(clientFd, clientHost, clientPort);
			connections_[clientFd] = conn;
			
			// Create HTTP connection state (Person 2)
			HTTPConnectionState* httpState = new HTTPConnectionState(conn);
			httpStates_[clientFd] = httpState;
			conn->setUserData(httpState);
			
			// Register with event loop (Person 1)
			eventLoop_.addFd(clientFd, true, false);
			
			std::cout << "New connection from " << clientHost << ":" << clientPort << std::endl;
		}
		
		void handleRead(int fd, Connection* conn) {
			// Read data (Person 1)
			ssize_t bytesRead = conn->read();
			
			if (bytesRead < 0) {
				conn->markForClose();
				return;
			}
			
			if (bytesRead == 0) {
				// EOF
				conn->markForClose();
				return;
			}
			
			// Process HTTP data (Person 2)
			HTTPConnectionState* httpState = httpStates_[fd];
			if (httpState) {
				Buffer& readBuffer = conn->getReadBuffer();
				ssize_t consumed = httpState->onDataReceived(readBuffer);
				
				if (consumed < 0) {
					// Parse error, response already set
				}
				
				// If request is complete, route it (Person 3)
				if (httpState->hasCompleteRequest()) {
					Request& request = httpState->getRequest();
					Response response = defaultRouter_.route(request);
					httpState->setResponse(response);
					
					// Update event loop to watch for write readiness
					eventLoop_.updateFd(fd, true, true);
				}
			}
		}
		
		void handleWrite(int fd, Connection* conn) {
			// Write data (Person 1)
			ssize_t bytesWritten = conn->write();
			
			if (bytesWritten < 0) {
				conn->markForClose();
				return;
			}
			
			// If all data written, check for keep-alive
			if (!conn->hasDataToWrite()) {
				HTTPConnectionState* httpState = httpStates_[fd];
				if (httpState && httpState->shouldKeepAlive()) {
					// Reset for next request
					httpState->resetForNextRequest();
					eventLoop_.updateFd(fd, true, false);
				} else {
					// Close connection
					conn->markForClose();
				}
			}
		}
		
		void removeConnection(int fd) {
			std::map<int, Connection*>::iterator connIt = connections_.find(fd);
			if (connIt != connections_.end()) {
				delete connIt->second;
				connections_.erase(connIt);
			}
			
			std::map<int, HTTPConnectionState*>::iterator httpIt = httpStates_.find(fd);
			if (httpIt != httpStates_.end()) {
				delete httpIt->second;
				httpStates_.erase(httpIt);
			}
			
			eventLoop_.removeFd(fd);
			::close(fd);
			
			std::cout << "Connection closed: " << fd << std::endl;
		}
};

int main(int argc, char** argv) {
	std::string configFile;
	if (argc > 1) {
		configFile = argv[1];
	}
	
	WebServer server;
	if (!server.init(configFile)) {
		std::cerr << "Failed to initialize server" << std::endl;
		return 1;
	}
	
	server.run();
	
	return 0;
}
