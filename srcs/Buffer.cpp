#include "../includes/Buffer.hpp"
#include <algorithm>
#include <cstring>

Buffer::Buffer() {
}

Buffer::~Buffer() {
}

void Buffer::append(const char* data, size_t len) {
	buffer_.insert(buffer_.end(), data, data + len);
}

void Buffer::append(const std::string& data) {
	append(data.c_str(), data.size());
}

size_t Buffer::read(char* dest, size_t maxLen) {
	size_t toRead = std::min(maxLen, buffer_.size());
	if (toRead > 0) {
		std::memcpy(dest, &buffer_[0], toRead);
		buffer_.erase(buffer_.begin(), buffer_.begin() + toRead);
	}
	return toRead;
}

std::string Buffer::readString(size_t maxLen) {
	size_t toRead = std::min(maxLen, buffer_.size());
	std::string result(&buffer_[0], toRead);
	buffer_.erase(buffer_.begin(), buffer_.begin() + toRead);
	return result;
}

size_t Buffer::peek(char* dest, size_t maxLen, size_t offset) const {
	if (offset >= buffer_.size()) {
		return 0;
	}
	size_t available = buffer_.size() - offset;
	size_t toRead = std::min(maxLen, available);
	if (toRead > 0) {
		std::memcpy(dest, &buffer_[offset], toRead);
	}
	return toRead;
}

size_t Buffer::size() const {
	return buffer_.size();
}

bool Buffer::empty() const {
	return buffer_.empty();
}

void Buffer::clear() {
	buffer_.clear();
}

size_t Buffer::find(const std::string& pattern, size_t startPos) const {
	if (startPos >= buffer_.size() || pattern.empty()) {
		return std::string::npos;
	}
	
	std::vector<char>::const_iterator it = std::search(
		buffer_.begin() + startPos,
		buffer_.end(),
		pattern.begin(),
		pattern.end()
	);
	
	if (it == buffer_.end()) {
		return std::string::npos;
	}
	return it - buffer_.begin();
}

const char* Buffer::data() const {
	if (buffer_.empty()) {
		return NULL;
	}
	return &buffer_[0];
}
