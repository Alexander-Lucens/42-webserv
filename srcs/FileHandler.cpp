
# include "FileHandler.hpp"
#include "Response.hpp"
#include "Request.hpp"

bool FileHandler::is_directory(const std::string &file_path)
{
	DIR* dir = opendir(file_path.c_str());
	if (dir != NULL)
	{
		closedir(dir);
		return true;
	}
	return false;
}

bool FileHandler::is_readable(const std::string &file_path)
{
	return (access(file_path.c_str(), R_OK) == 0);
}

bool FileHandler::file_exists(const std::string &file_path)
{
	std::ifstream file(file_path.c_str());
	if (!file.is_open())
		return false;
	file.close();
	return true;
}

/* Loads html files dynamically through given file path reference*/
std::string FileHandler::load_file(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
		return "";

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0)
		return "";

	if (static_cast<size_t>(size) > MAX_FILE_SIZE)
		return "";

	std::string content;
	content.resize(size);

	if (!file.read(&content[0], size))
		return "";

	return content;
}

/* Processes content files e.g. css */
std::string FileHandler::find_content_type(const std::string &content_file_path)
{
	// text types
	if (content_file_path.find(".html") != std::string::npos)
		return "text/html";
	if (content_file_path.find(".txt") != std::string::npos)
		return "text/plain";
	if (content_file_path.find(".css") != std::string::npos)
		return "text/css";
	if (content_file_path.find(".csv") != std::string::npos)
		return "text/csv";

	// javaScript
	if (content_file_path.find(".js") != std::string::npos)
		return "application/javascript";

	// json
	if (content_file_path.find(".json") != std::string::npos)
		return "application/json";

	// images
	if (content_file_path.find(".jpg") != std::string::npos || content_file_path.find(".jpeg") != std::string::npos)
		return "image/jpeg";
	if (content_file_path.find(".png") != std::string::npos)
		return "image/png";
	if (content_file_path.find(".gif") != std::string::npos)
		return "image/gif";
	if (content_file_path.find(".svg") != std::string::npos)
		return "image/svg+xml";
	if (content_file_path.find(".ico") != std::string::npos)
		return "image/x-icon";

	// documents
	if (content_file_path.find(".pdf") != std::string::npos)
		return "application/pdf";
	if (content_file_path.find(".zip") != std::string::npos)
		return "application/zip";

	// audio/Video
	if (content_file_path.find(".mp3") != std::string::npos)
		return "audio/mpeg";
	if (content_file_path.find(".mp4") != std::string::npos)
		return "video/mp4";

	return ("text/html");
	}

bool FileHandler::save_uploaded_file(const std::string& file_path, const std::string& content)
{
	std::ofstream outfile(file_path.c_str(), std::ios::binary);
	if (!outfile.is_open())
		return false;
	
	outfile.write(content.c_str(), content.length());
	outfile.close();
	return true;
}
std::string FileHandler::html_escape(const std::string& input)
{
	std::string out;
	out.reserve(input.size());

	for (size_t i = 0; i < input.size(); ++i) {
		switch (input[i]) {
			case '&':  out += "&amp;";  break;
			case '<':  out += "&lt;";   break;
			case '>':  out += "&gt;";   break;
			case '"':  out += "&quot;"; break;
			case '\'': out += "&#39;";  break;
			default:   out += input[i]; break;
		}
	}
	return out;
}

std::string FileHandler::handle_autoindex(const std::string &normalized_html_path, const std::string &directory_path)
{
	//  pointer to a directory stream
	DIR* directory = opendir(directory_path.c_str());
	if (!directory)
		return ("");

	std::vector<std::string> file_entries;
	// structure representing a single entry 
	struct dirent* entry;
	while ((entry = readdir(directory)) != NULL)
	{
		// ignore dot files
		if (entry->d_name[0] == '.')
			continue;
		// get filename
		file_entries.push_back(entry->d_name);
	}
	closedir(directory);

	// sort alphabetically 
	std::sort(file_entries.begin(), file_entries.end());

	std::ostringstream html;
	html << "<!DOCTYPE html>\n"
			<< "<html><head><meta charset=\"utf-8\">"
			<< "<title>Index of the uploads folder</title>"
			<< "<style>"
			<< "body { background-color: black; color: #13d019; font-family: Arial, sans-serif; display: flex; flex-direction: column; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }"
			<< "div { text-align: center; }"
			<< "h2 { color: #13d019; }"
			<< "a { color: #13d019; text-decoration: none; }"
			<< "a:hover { text-decoration: underline; }"
			<< "ul { display: inline-block; text-align: left; }"
			<< "</style>"
			<< "</head><body>\n"
			<< "<div>\n"
			<< "<h1>Index of the uploads folder</h1>\n"
			<< "<div>\n"
			<< "<ul>\n";

	for (size_t i = 0; i < file_entries.size(); ++i)
	{
		std::string filename = file_entries[i];
		std::string href_link = normalized_html_path;
		if (!href_link.empty() && href_link[href_link.size() - 1] != '/')
			href_link += '/';
		href_link += filename;
		html << "<li><a href=\"" << href_link << "\">"
				<< html_escape(filename) << "</a></li>\n";
	}
	html << "</ul></body></html>";
	return html.str();
}

/* Extracts the filename from multipart form-data request body.
Returns extracted filename from filename= field, or uploaded_file as default. */
std::string FileHandler::get_filename_from_multipart(const std::string &body)
{
	std::string filename_marker = "filename=\"";
	size_t pos = body.find(filename_marker);

	if (pos != std::string::npos) {
		pos += filename_marker.length();
		size_t end = body.find("\"", pos);
		if (end != std::string::npos)
			return body.substr(pos, end - pos);
	}
	return "uploaded_file";
}

/*  Extracts file content from multipart form-data request body between boundaries. */
std::string FileHandler::get_file_content(const std::string &body,
                                          const std::string &boundary)
{
	const std::string boundary_marker = "--" + boundary;
	const std::string boundary_end = boundary_marker + "--";

	// find first boundary
	size_t pos = body.find(boundary_marker);
	if (pos == std::string::npos)
		return "";

	// skip boundary + CRLF
	pos = body.find("\r\n", pos);
	if (pos == std::string::npos)
		return "";
	pos += 2;

	// skip headers end 
	size_t headers_end = body.find("\r\n\r\n", pos);
	if (headers_end == std::string::npos)
		return "";
	size_t content_start = headers_end + 4;

	// find next boundary 
	size_t next_boundary = body.find("\r\n" + boundary_marker, content_start);
	if (next_boundary == std::string::npos)
		return "";

	size_t content_end = next_boundary;
	return body.substr(content_start, content_end - content_start);
}


std::string FileHandler::extract_form_data(const std::string &body, const std::string &field_name)
{
	std::string search_key = field_name + "=";
	size_t pos = body.find(search_key);

	if (pos == std::string::npos)
		return "";

	// Move past the "field_name=" part
	pos += search_key.length();

	// Find the end of the value (either & or end of string)
	size_t end = body.find("&", pos);
	if (end == std::string::npos)
		end = body.length();

	return body.substr(pos, end - pos);
}
