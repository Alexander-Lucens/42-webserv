#include "../includes/HTTPSerializer.hpp"
#include <sstream>

HTTPSerializer::HTTPSerializer() {
}

HTTPSerializer::~HTTPSerializer() {
}

bool HTTPSerializer::serialize(const Response& response, Buffer& buffer) {
	if (!response.isReady()) {
		return false;
	}
	
	std::string httpResponse = response.toString();
	buffer.append(httpResponse);
	
	return true;
}

bool HTTPSerializer::needsSerialization(const Response& response) const {
	return response.isReady();
}
