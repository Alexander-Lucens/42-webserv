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