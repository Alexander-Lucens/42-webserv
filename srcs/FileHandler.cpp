# include "FileHandler.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include <dirent.h>
# include <sys/stat.h>

#define to_string(x) static_cast<std::ostringstream&>(std::ostringstream() << (x)).str()

/* 
	Checks if path is a directory. 
	Returns true if directory, false otherwise.
*/
bool FileHandler::is_directory(const std::string &file_path)
{
	DIR* directory = opendir(file_path.c_str());
	if (directory != NULL)
	{
		closedir(directory);
		return true;
	}
	return false;
}
/* 
	Checks if file/directory has read permissions. 
	Returns true if readable, false otherwise.
*/
bool FileHandler::is_readable(const std::string &file_path)
{
	struct stat file_info;
	if (stat(file_path.c_str(), &file_info) != 0)
		return false;

	// check for directories
	if (S_ISDIR(file_info.st_mode))
	 	return ((file_info.st_mode & S_IXUSR) != 0 && access(file_path.c_str(), X_OK) == 0);

    // check read permissions for files
    if ((file_info.st_mode & S_IRUSR) == 0)
    	return false;

    return (access(file_path.c_str(), R_OK) == 0);
}

/* 
	Checks if file has write permissions.
	Returns true if writable, false otherwise.
*/
bool FileHandler::is_writable(const std::string &file_path)
{
	struct stat file_info;
	if (stat(file_path.c_str(), &file_info) != 0)
		return false;

	// check if no write permissions for owner
	if ((file_info.st_mode & S_IWUSR) == 0)
		return false;
	if (access(file_path.c_str(), W_OK) != 0)
		return false;

	return true;
}

/* 
	Checks if the given file exists. 
	Returns true if exists, false otherwise.
*/
bool FileHandler::file_exists(const std::string &file_path)
{
	// system call with stat to generate struct with metadata
	struct stat file_info;
    return (stat(file_path.c_str(), &file_info) == 0);
}

/* 
	Loads and returns contents of a file as a string. 
	Returns empty string if file cannot be read.
*/
std::string FileHandler::load_file(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		LOG_WARNING("load_file: cannot open file '" << path << "'");
		return "";
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0) {
		LOG_WARNING("load_file: file '" << path << "' is empty or invalid");
		return "";
	}
	if (static_cast<size_t>(size) > MAX_FILE_SIZE) {
		LOG_WARNING("load_file: file '" << path << "' exceeds max size of " << MAX_FILE_SIZE << " bytes");
		return "";
	}

	std::string content;
	content.resize(size);
	if (!file.read(&content[0], size)) {
		LOG_WARNING("load_file: failed to read file '" << path << "'");
		return "";
	}

	return content;
}


static std::string get_extension(const std::string &path)
{
	size_t index = path.rfind('.');
	if (index == std::string::npos) {
		LOG_WARNING("get_extension:" << path << "' not found");
		return "";
	}
	return path.substr(index);
}
/* 
	Finds MIME type based on file extension. 
	Returns MIME type string or "application/octet-stream" as default.
*/
std::string FileHandler::find_content_type(const std::string &path)
{
	static std::map<std::string, std::string> mime_types;
	if (mime_types.empty())
	{
		mime_types[".html"] = "text/html";
		mime_types[".txt"]  = "text/plain";
		mime_types[".css"]  = "text/css";
		mime_types[".csv"]  = "text/csv";
		mime_types[".js"]   = "application/javascript";
		mime_types[".json"] = "application/json";
		mime_types[".jpg"]  = "image/jpeg";
		mime_types[".jpeg"] = "image/jpeg";
		mime_types[".png"]  = "image/png";
		mime_types[".gif"]  = "image/gif";
		mime_types[".svg"]  = "image/svg+xml";
		mime_types[".ico"]  = "image/x-icon";
		mime_types[".pdf"]  = "application/pdf";
		mime_types[".zip"]  = "application/zip";
		mime_types[".mp3"]  = "audio/mpeg";
		mime_types[".mp4"]  = "video/mp4";
		mime_types[".sh"]   = "application/x-sh";
		mime_types[".exe"]  = "application/x-msdownload";
		mime_types[".bin"]  = "application/octet-stream";
	}

	std::string extension = get_extension(path);
	std::map<std::string, std::string>::iterator match = mime_types.find(extension);
	if (match != mime_types.end())
		return match->second;

	return "application/octet-stream";
}
/* 
	Saves uploaded file content to file path. 
	Returns true on success, false on failure.
	*/
bool FileHandler::save_uploaded_file(const std::string& file_path, const std::string& content)
{
	std::ofstream outfile(file_path.c_str(), std::ios::binary);
	if (!outfile.is_open())
		return false;
	
	outfile.write(content.c_str(), content.length());
	outfile.close();
	return true;
}

/* 
	Converts HTML special characters to prevent injection attacks. 
	Returns the escaped string. 
*/
std::string FileHandler::convert_html_chars(const std::string& input)
{
	std::string result;
	result.reserve(input.size());

	for (size_t i = 0; i < input.size(); ++i) {
		switch (input[i]) {
			case '&':  result += "&amp;";  break;
			case '<':  result += "&lt;";   break;
			case '>':  result += "&gt;";   break;
			case '"':  result += "&quot;"; break;
			case '\'': result += "&#39;";  break;
			default:   result += input[i]; break;
		}
	}
	return result;
}
/* 
	Decodes URL-encoded strings (e.g., %20 to space, %C3%A4 to ä).
	Returns decoded string.
*/
std::string FileHandler::decode_url(const std::string &encoded)
{
	std::string decoded;
	decoded.reserve(encoded.size());

	for (size_t i = 0; i < encoded.size(); ++i) {
		if (encoded[i] == '%' && i + 2 < encoded.size()) {
			// convert hex characters to decimal
			std::string hex = encoded.substr(i + 1, 2);
			char byte = static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
			decoded += byte;
			i += 2;
		} else if (encoded[i] == '+') {
			decoded += ' ';
		} else {
			decoded += encoded[i];
		}
	}
	return decoded;
}
/* 
	Generates an HTML directory listing for given directory path. 
	Returns HTML string or empty string on error.
*/
std::string FileHandler::handle_autoindex(const std::string &normalized_html_path, const std::string &directory_path)
{
	//  pointer to a directory stream
	DIR* directory = opendir(directory_path.c_str());
	if (!directory)
	{
		LOG_ERROR("Cannot open directory: " << directory_path);
		return ("");
	}

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
			<< "body { background-color: black; color: #fffff9; font-family: Arial, sans-serif; display: flex; flex-direction: column; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }"
			<< "div { text-align: center; }"
			<< "h2 { color: #fffff9; }"
			<< "a { color: #fdfffc; text-decoration: none; }"
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
				<< convert_html_chars(filename) << "</a></li>\n";
	}
	html << "</ul></body></html>";
	return html.str();
}

/* Extracts the filename from multipart form-data request body.
Returns extracted filename from filename= field, or uploaded_file as default. */
std::string FileHandler::get_filename_from_multipart(const std::string &body)
{
	std::string filename_marker = "filename=\"";
	size_t index = body.find(filename_marker);

	if (index != std::string::npos) {
		index += filename_marker.length();
		size_t closing_quote = body.find("\"", index);
		if (closing_quote != std::string::npos) {
			std::string filename = body.substr(index, closing_quote - index);
			return decode_url(filename);
		}
	}
	LOG_WARNING("get_filename_from_multipart: filename not found, using default");
	return "unnamed_upload";
}

/*  
	Extracts file content from multipart form-data request body between boundaries. 
	Returns filename or "uploaded_file" as default.
	*/
std::string FileHandler::get_file_content(const std::string &body, const std::string &boundary)
{
	const std::string boundary_marker = "--" + boundary;
	const std::string boundary_end = boundary_marker + "--";

	// find first boundary
	size_t index = body.find(boundary_marker);
	if (index == std::string::npos) {
		LOG_WARNING("get_file_content: boundary marker not found");
		return "";
	}

	// skip boundary + CRLF
	index = body.find("\r\n", index);
	if (index == std::string::npos){
		LOG_WARNING("get_file_content: CRLF after boundary not found");
		return "";
	}
	index += 2;

	// skip headers end 
	size_t headers_end = body.find("\r\n\r\n", index);
	if (headers_end == std::string::npos) {
		LOG_WARNING("get_file_content: headers end not found");
		return "";
	}
	size_t content_start = headers_end + 4;

	// find next boundary 
	size_t next_boundary = body.find("\r\n" + boundary_marker, content_start);
	if (next_boundary == std::string::npos) {
		LOG_WARNING("get_file_content: closing boundary not found");
		return "";
	}

	size_t content_end = next_boundary;
	return body.substr(content_start, content_end - content_start);
}

/* 
	Extracts value of a form field from URL-encoded form data.
	Returns file content string or empty string if not found 
*/
std::string FileHandler::extract_form_data(const std::string &body, const std::string &field_name)
{
	std::string query_key = field_name + "=";
	size_t index = body.find(query_key);

	if (index == std::string::npos) {
		LOG_WARNING("extract_form_data:" << field_name << "' not found");
		return "";
	}

	// move past "field_name=" part
	index += query_key.length();

	// find the end of value, either & or end of string
	size_t value_end = body.find("&", index);
	if (value_end == std::string::npos)
		value_end = body.length();
	return body.substr(index, value_end - index);
}

std::string FileHandler::generate_directory_listing(const std::string &directory, const std::string &uri)
{
    std::string html = get_html_header("List of " + uri);
	
	html +=std::string("<body><div class=\"header\"><h1>📁 " + uri + "</h1></div>" \
        + "<div class=\"container\"><table>" \
        + "<tr><th>Name</th><th class=\"size\">Size</th><th>Type</th><th class=\"date\">Modified</th></tr>");    
    // Parent directory
    if (uri != "/" && uri != "/uploads") {
        std::string parent = uri;
        // Remove trailing slash
        if (parent.length() > 0 && parent[parent.length() - 1] == '/') {
            parent = parent.substr(0, parent.length() - 1);
        }
        // Find last slash
        size_t last_slash = parent.find_last_of('/');
        if (last_slash != std::string::npos) {
            parent = parent.substr(0, last_slash + 1);
        } else {
            parent = "/";
        }
        html += "<tr><td><a href=\"" + parent + "\">↑ Parent Directory</a></td>"
                "<td></td><td>DIR</td><td></td></tr>";
    }
    
    // Read directory
    DIR *dir = opendir(directory.c_str());
    if (!dir) {
        html += "</table></div><footer></footer></body></html>";
        return html;
    }
    
    struct dirent *entry;
    std::vector<std::string> entries;
    
    // Collect entries
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        entries.push_back(entry->d_name);
    }
    closedir(dir);
    
    // Sort alphabetically
    std::sort(entries.begin(), entries.end());
    
    // Generate rows
    for (size_t i = 0; i < entries.size(); ++i) {
        std::string name = entries[i];
        
        // Build full path
        std::string full_path = directory;
        if (full_path.length() > 0 && full_path[full_path.length() - 1] != '/') {
            full_path += "/";
        }
        full_path += name;
        
        struct stat st;
        if (stat(full_path.c_str(), &st) != 0) continue;
        
        // Build URL
        std::string url = uri;
        if (url.length() > 0 && url[url.length() - 1] != '/') {
            url += "/";
        }
        url += name;
        
        // Determine type and format size
        std::string type_str = "FILE";
        std::string size_str = to_string(st.st_size) + " B";
        std::string icon = "📄";
        
        if (S_ISDIR(st.st_mode)) {
            icon = "📁";
            type_str = "DIR";
            size_str = "-";
            url += "/";
        } else if (st.st_size > 1024) {
            size_str = to_string(st.st_size / 1024) + " KB";
        }
        
        // Format date
        struct tm *tm_info = localtime(&st.st_mtime);
        char date_buf[32];
        strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M", tm_info);
        
        // Add row
        html += "<tr><td><a href=\"" + url + "\">" + icon + " " + name;
        if (S_ISDIR(st.st_mode)) {
            html += "/";
        }
        html += "</a></td>";
        html += "<td class=\"size\">" + size_str + "</td>";
        html += "<td>" + type_str + "</td>";
        html += "<td class=\"date\">" + std::string(date_buf) + "</td>";
        html += "</tr>";
    }
    
    html += "</table></div>"
            "<footer><p>&copy; 2026 webserv</p></footer>"
            "</body></html>";
    
    return html;
}