#include "Utils.hpp"
#include "ConfigParser.hpp"
#include <cctype>
#include <string>
#include <algorithm>

/* Returns current time and date */
std::string Utils::get_http_date() 
{
	std::time_t now = std::time(NULL);
	std::tm* gmt = std::gmtime(&now);

	char buffer[128];
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return (std::string(buffer));
}


std::string Utils::extract_boundary(const std::string& content_type)
{
	size_t boundary_pos = content_type.find("boundary=");
	if (boundary_pos == std::string::npos)
		return "";
	return content_type.substr(boundary_pos + 9);
}


int Utils::get_errno_code()
{
	switch (errno)
	{
		case ENOENT:
			return 404;
		case EACCES:
		case EPERM:
			return 403;
		default:
			return 500;
	}
}


bool Utils::is_valid_uri(const std::string& uri)
{
	if (uri.empty()) {
		LOG_ERROR("URI validation: Empty URI");
		return false;
	}
	if (uri[0] != '/') {
		LOG_ERROR("URI validation: Invalid URI format - must start with /");
		return false;
	}

	if (uri.find('\0') != std::string::npos || uri.find("%00") != std::string::npos) {
		LOG_ERROR("URI validation: Null byte detected in URI");
		return false;
	}

	if (uri.find("..") != std::string::npos) {
		LOG_ERROR("URI validation: Path traversal attempt detected: " << uri);
		return false;
	}
	if (uri.find("%2e%2e") != std::string::npos || uri.find("%2E%2E") != std::string::npos) {
		LOG_ERROR("URI validation: Encoded path traversal attempt detected: " << uri);
		return false;
	}
    return true;
}
static int to_lower_safe(int c)
{
	return std::tolower((unsigned char)c);
}

static int to_upper_safe(int c)
{
	return std::toupper((unsigned char)c);
}

std::string Utils::lower_case(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), to_lower_safe);
	return str;
}

std::string Utils::upper_case(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), to_upper_safe);
	return str;
}

std::string Utils::dash_to_underscore(std::string str)
{
	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		if (str[i] == '-')
		{
			str[i] = '_';
		}
	}
	return str;
}
