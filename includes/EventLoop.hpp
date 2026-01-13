#pragma once

#include <vector>

// Forward declarations
class Connection;

// Person 1: Event loop interface
// This is the core I/O multiplexing interface
class EventLoop {
	public:
		EventLoop();
		~EventLoop();
		
		// Initialize event loop (epoll/kqueue/select)
		bool init();
		
		// Register/unregister file descriptors
		bool addFd(int fd, bool read, bool write);
		bool removeFd(int fd);
		bool updateFd(int fd, bool read, bool write);
		
		// Run event loop (blocks until events occur)
		// Returns number of events processed
		int poll(int timeoutMs = -1);
		
		// Get events for a specific fd
		bool isReadReady(int fd) const;
		bool isWriteReady(int fd) const;
		
		// Cleanup
		void cleanup();
		
	private:
		// Platform-specific implementation
		// On macOS: use kqueue
		// On Linux: use epoll
		// Fallback: use select
		
		int kq_; // kqueue descriptor (macOS)
		std::vector<struct kevent> events_;
		std::vector<struct kevent> changes_;
		
		// Helper to detect platform
		bool isMacOS() const;
		bool isLinux() const;
		
		// Platform-specific implementations
		bool initKqueue();
		bool initEpoll();
		bool initSelect();
		
		int pollKqueue(int timeoutMs);
		int pollEpoll(int timeoutMs);
		int pollSelect(int timeoutMs);
};
