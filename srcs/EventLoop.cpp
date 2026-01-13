#include "../includes/EventLoop.hpp"
#include <sys/event.h> // kqueue
#include <sys/select.h> // select fallback
#include <unistd.h>
#include <cerrno>
#include <cstring>

EventLoop::EventLoop() : kq_(-1) {
}

EventLoop::~EventLoop() {
	cleanup();
}

bool EventLoop::init() {
	if (isMacOS()) {
		return initKqueue();
	} else if (isLinux()) {
		return initEpoll();
	} else {
		return initSelect();
	}
}

bool EventLoop::initKqueue() {
	kq_ = kqueue();
	if (kq_ < 0) {
		return false;
	}
	events_.resize(128);
	changes_.clear();
	return true;
}

bool EventLoop::initEpoll() {
	// TODO: Person 1 should implement epoll for Linux
	// For now, fallback to select
	return initSelect();
}

bool EventLoop::initSelect() {
	// Select doesn't need initialization
	return true;
}

bool EventLoop::addFd(int fd, bool read, bool write) {
	if (isMacOS() && kq_ >= 0) {
		struct kevent ev;
		if (read) {
			EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
			changes_.push_back(ev);
		}
		if (write) {
			EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			changes_.push_back(ev);
		}
		return true;
	}
	// For select, we just track fds (implementation in pollSelect)
	return true;
}

bool EventLoop::removeFd(int fd) {
	if (isMacOS() && kq_ >= 0) {
		struct kevent ev;
		EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		changes_.push_back(ev);
		EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		changes_.push_back(ev);
		return true;
	}
	return true;
}

bool EventLoop::updateFd(int fd, bool read, bool write) {
	// Remove and re-add
	removeFd(fd);
	return addFd(fd, read, write);
}

int EventLoop::poll(int timeoutMs) {
	if (isMacOS() && kq_ >= 0) {
		return pollKqueue(timeoutMs);
	} else if (isLinux()) {
		return pollEpoll(timeoutMs);
	} else {
		return pollSelect(timeoutMs);
	}
}

int EventLoop::pollKqueue(int timeoutMs) {
	struct timespec timeout;
	struct timespec* timeoutPtr = NULL;
	
	if (timeoutMs >= 0) {
		timeout.tv_sec = timeoutMs / 1000;
		timeout.tv_nsec = (timeoutMs % 1000) * 1000000;
		timeoutPtr = &timeout;
	}
	
	int numEvents = kevent(kq_, &changes_[0], changes_.size(),
						   &events_[0], events_.size(), timeoutPtr);
	changes_.clear();
	
	return numEvents;
}

int EventLoop::pollEpoll(int timeoutMs) {
	// TODO: Person 1 should implement epoll
	(void)timeoutMs;
	return 0;
}

int EventLoop::pollSelect(int timeoutMs) {
	// TODO: Person 1 should implement select properly
	// This is a placeholder
	(void)timeoutMs;
	return 0;
}

bool EventLoop::isReadReady(int fd) const {
	// Check events_ for this fd
	for (size_t i = 0; i < events_.size(); ++i) {
		if (events_[i].ident == (unsigned int)fd && 
			events_[i].filter == EVFILT_READ) {
			return true;
		}
	}
	return false;
}

bool EventLoop::isWriteReady(int fd) const {
	// Check events_ for this fd
	for (size_t i = 0; i < events_.size(); ++i) {
		if (events_[i].ident == (unsigned int)fd && 
			events_[i].filter == EVFILT_WRITE) {
			return true;
		}
	}
	return false;
}

void EventLoop::cleanup() {
	if (kq_ >= 0) {
		::close(kq_);
		kq_ = -1;
	}
	events_.clear();
	changes_.clear();
}

bool EventLoop::isMacOS() const {
#ifdef __APPLE__
	return true;
#else
	return false;
#endif
}

bool EventLoop::isLinux() const {
#ifdef __linux__
	return true;
#else
	return false;
#endif
}
