# include "Response.hpp"
# include "Request.hpp"

/* 
HttpRequest (from parser)
	↓
Validation
	↓
Executor / Dispatcher
	↓
- Path resolution
- Method handling (GET/POST/DELETE)
- Error handling (handle_error)
	↓
Response object
	↓
Serializer (Response → raw HTTP)
	↓
socket sends it
*/

// Default constructor
Response::Response() : _status_code(200), version("HTTP/1.1") {}

// Copy constructor
Response::Response(const Response& other)
	:	_status_code(other._status_code),
		_html_body(other._html_body),
		_headers(other._headers),
		version(other.version) {}


// Copy assignment operator
Response& Response::operator=(const Response& other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
		_html_body = other._html_body;
		_headers = other._headers;
		version = other.version;
	}
	return (*this);
}

// No custom containers that need explicit destruction
Response::~Response() {}


/* Setter */
void Response::set_status(int status_code)
{
	_status_code = status_code;
}

void Response::set_header(const std::string &key, const std::string &value)
{	
	_headers[key] = value;
}

Response Response::handle_get(const Request& request)
{
	Response response;

	std::string normalized_html_path = normalize_path(request.path);
	std::string file_path = "www/base_page" + normalized_html_path; // get dir through config later

	if (!FileHandler::file_exists(file_path))
		return handle_error(404);
	/* if (FileHandler::is_directory(file_path))
		return handle_error(405);  */// I need to add logic for : handle_autoindex(file_path)
	if (!FileHandler::is_readable(file_path))
		return handle_error(403);

	std::string html_content = FileHandler::load_file(file_path);
	if(html_content.empty())
		return (handle_error(404)); 

	response.set_status(200);
	response.set_header("Date", get_http_date());
	response.set_header("Server", SERVER);
	response.set_header("Content-Type", FileHandler::find_content_type(file_path));
	response.set_body(html_content);

	return (response);
}

Response Response::handle_post(const Request& request)
{
	Response response;

	// Validate request 
	std::string content_length_val = request.getHeader("Content-Length");
	if (content_length_val.empty() || request.body.empty())
		return(handle_error(400));
	
	int content_len = std::atoi(content_length_val.c_str());
	if ((int)request.body.length() != content_len)
		return (handle_error(400));
	if (request.path == "/submit")
    {
        std::string body = "<h1>200 OK</h1><p>Form data received: " + request.body + "</p>";
        return (response_body(200, body));
    }
	else if (request.path == "/upload")
	{
		std::string content_type = request.getHeader("Content-Type");
		std::string boundary;

		size_t boundary_pos = content_type.find("boundary=");
		if (boundary_pos != std::string::npos) {
			boundary = content_type.substr(boundary_pos + 9);
			boundary = "--" + boundary;
		}
		// Extract filename and file content from multipart body
		std::string file_name = get_filename_from_multipart(request.body);
		std::string file_content = get_file_content(request.body, boundary);

		if (file_content.empty())
			return (handle_error(400));

		// Save uploaded file
		std::string upload_file_path = "www/uploads/" + file_name;
		std::ofstream outfile(upload_file_path.c_str(), std::ios::binary);
		if (!outfile.is_open())
			return (handle_error(500));

		outfile.write(file_content.c_str(), file_content.length());
		outfile.close();

		std::string upload_body = "<h1>201 Created</h1><p>File '" + file_name + "' uploaded successfully</p>";
		response = response_body(201, upload_body);
		response.set_header("Location", "/uploads/" + file_name);
		return (response);
	}
	return (handle_error(405));
}

Response Response::handle_delete(const Request &request)
{
	std::string file_path = "www/base_page" + request.path;
	if (!FileHandler::file_exists(file_path))
		return (handle_error(404));
	if (std::remove(file_path.c_str()) != 0)
		return (handle_error(500));

	std::string file_name = request.path;
	size_t last_slash = file_name.find_last_of("/");
	if (last_slash != std::string::npos)
		file_name = file_name.substr(last_slash + 1);

    std::string body = "<h1>200 OK</h1><p>File '" + file_name + "' successfully deleted</p>";
	return ((response_body(200, body)));
}

Response Response::handle_error(const int error_code)
{
	std::string error_file;

	switch(error_code)
	{
		case 400:
			error_file = "www/errors/400.html";
			break;
		case 401:
			error_file = "www/errors/401.html";
			break;
		case 403:
			error_file = "www/errors/403.html";
			break;
		case 404:
			error_file = "www/errors/404.html";
			break;
		case 405:
			error_file = "www/errors/405.html";
			break;
		case 500:
			error_file = "www/errors/500.html";
			break;
		default:
			error_file= "www/errors/501.html";
	}

	std::string html_error_file = FileHandler::load_file(error_file); 
	return ((response_body(error_code, html_error_file)));
}

/* Filters request type and requested function  */
Response Response::handle_request(const Request &request)
{
	if (request.method == "GET")
		return (handle_get(request));
	if (request.method == "POST")
		return (handle_post(request));
	if (request.method == "DELETE")
		return (handle_delete(request));
	return (handle_error(501));
	
}

/* Returns current time and date */
std::string Response::get_http_date() 
{
	std::time_t now = std::time(NULL);
	std::tm* gmt = std::gmtime(&now);

	char buffer[128];
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return (std::string(buffer));
}


void Response::set_body(const std::string &html_body)
{
	_html_body = html_body;
	std::stringstream ss;
	ss << _html_body.size();
	_headers["Content-Length"] = ss.str();
}

/* Sends response to socket (creates one liner) */
std::string Response::serialize()
{
	std::ostringstream out;
	out << version << " " << _status_code << " " << reason_message(_status_code) << NEW_LINE;

	// write all headers
	for (std::map<std::string, std::string>::const_iterator map_item = this->_headers.begin();
	     map_item != this->_headers.end();
	     ++map_item)
	{
		out << map_item->first << ": " << map_item->second << NEW_LINE;
	}

	out << NEW_LINE;
	out << this->_html_body;
	return out.str();
}

/* Matches error code and returns error message  */
std::string Response::reason_message(int code)
{
	switch (code)
	{
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 413: return "Payload Too Large";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		default:  return "Unknown";
	}
}

/* Extracts the filename from multipart form-data request body.

Returns extracted filename from filename= field, or uploaded_file as default. */
std::string Response::get_filename_from_multipart(const std::string &body)
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
std::string Response::get_file_content(const std::string &body, const std::string &boundary)
{
	size_t start = body.find(boundary);
	if (start == std::string::npos)
		return "";

	size_t content_start = body.find("\r\n\r\n", start);
	if (content_start == std::string::npos)
		return "";
	content_start += 4;

	// Find end boundary
	size_t content_end = body.find("\r\n" + boundary, content_start);
	if (content_end == std::string::npos)
		return "";

	return body.substr(content_start, content_end - content_start);
}
/* Returns response body information */
Response Response::response_body(const int &error_code, const std::string &body)
{
	Response response; 

	response.set_status(error_code);
	response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Date", get_http_date());
    response.set_header("Server", SERVER);
	response.set_body(body);

	return (response); 
}

/* Normalizes request paths by removing query strings and setting default file. */
std::string Response::normalize_path(const std::string &path)
{
    std::string normalized = path;
    size_t query_start = normalized.find("?");
    if (query_start != std::string::npos)
        normalized = normalized.substr(0, query_start);
    
    if (normalized.empty() || normalized == "/")
        normalized = "/index.html";
    
    return normalized;
}

/* Response handle_autoindex(const std::string& path)
{
	// Check directory exists and is readable
	// Generate simple HTML table with files
	// Include parent directory link
	// Format file sizes
	// Set Content-Type header
	// Return 403 if autoindex is disabled in config
} */