#include "Utils.hpp"

/* Returns current time and date */
std::string Utils::get_http_date() 
{
	std::time_t now = std::time(NULL);
	std::tm* gmt = std::gmtime(&now);

	char buffer[128];
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return (std::string(buffer));
}

/* Normalizes request paths by removing query strings and setting default file. */
std::string Utils::normalize_path(const std::string &path)
{
	std::string normalized = path;
	size_t query_start = normalized.find("?");
	if (query_start != std::string::npos)
		normalized = normalized.substr(0, query_start);

	if (normalized.empty() || normalized == "/")
		normalized = "/index.html";

	return normalized;
}

std::string Utils::extract_boundary(const std::string& content_type)
{
	size_t boundary_pos = content_type.find("boundary=");
	if (boundary_pos == std::string::npos)
		return "";
	return content_type.substr(boundary_pos + 9);
}