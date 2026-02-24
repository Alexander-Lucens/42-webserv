# include "Response.hpp"
# include "Request.hpp"
# include "CGI.hpp"
# include "Logger.hpp"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Default constructor
Response::Response() : _status_code(200), version("HTTP/1.1") {}

// Copy constructor
Response::Response(const Response& other)
	:	_status_code(other._status_code),
		_html_body(other._html_body),
		_headers(other._headers),
		_method(other._method),
		_request_uri(other._request_uri),
		version(other.version) {}

// Copy assignment operator
Response& Response::operator=(const Response& other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
		_html_body = other._html_body;
		_headers = other._headers;
		_method = other._method;
		_request_uri = other._request_uri;
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

void Response::set_method(const Request &request)
{	
	_method = request.method;
}

/**
 * @brief Set config implementation
 * 
 * @param request 
 */
void Response::set_config(const Request &request) {
	std::string host = request.getHeader("Host");
    if (host.empty()) {
        host = DEFAULT_HOST;
	}
	_config = &ConfigParser::get_instance().get_config(request.port, host);
}

/* Filters request type and requested function  */
Response Response::handle_request(const Request &request)
{
	// LOG_INFO("=== Response ===");
	// LOG_INFO("*IMPORTANT* URI: " << request.uri);
	// LOG_INFO("*IMPORTANT* Path: " << request.path);
	set_config(request);
	set_method(request);
	 _request_uri = request.uri;

	if (request.uri == "/old-page")
		return handle_redirect();
	if (request.method == "GET")
		return (handle_get(request));
	if (request.method == "POST")
		return (handle_post(request));
	if (request.method == "DELETE")
		return (handle_delete(request));

	LOG_WARNING("Unsupported method: " << request.method);
	return (handle_error(501));
}

Response Response::handle_get(const Request& request)
{
	std::string uri = FileHandler::decode_url(request.uri);
	std::string path = FileHandler::decode_url(request.path); 
	if (uri == "/error-404")
		return handle_error(404);
	if (uri == "/error-403")
		return handle_error(403);
	if (uri == "/error-400")
        return handle_error(400);

	if	(path == "/cgi-bin/script.py")
		return handle_get_cgi(request, *this, PYTHON);

	if	(path == "/cgi-bin/rust_program")
		return handle_get_cgi(request, *this, RUST);

	Response response;
	std::string file_path = file_path_check(uri);
	
	if (FileHandler::is_directory(file_path))
		return handle_directory(uri, file_path);
	int status_code = validate_file_path(file_path);
	if (status_code != 0)
		return handle_error(status_code);

	std::string body = FileHandler::load_file(file_path);
	if(body.empty())
	{
		LOG_ERROR("File not loading: " << file_path);
		return (handle_error(Utils::get_errno_code())); 
	}
	if (uri.find("/uploads/") == 0)
		response.set_download_header(uri);
	
	response.set_status(200);
	response.set_header("Date", Utils::get_http_date());
	response.set_header("Server", SERVER);
	response.set_header("Content-Type", FileHandler::find_content_type(file_path));
	response.set_body(body);
	return (response);
}


void Response::set_download_header(const std::string &path)
{
	size_t last_slash = path.find_last_of('/');
	std::string file_name;

	if (last_slash != std::string::npos)
		file_name = path.substr(last_slash + 1);
	else
		file_name = path;
	set_header("Content-Disposition", "attachment; filename=\"" + file_name + "\"");
	}

Response Response::handle_post(const Request& request)
{
	if (request.uri.find("/submit") == 0)
		return handle_post_submit(request);

	if (request.uri.find("/upload") == 0)
		return handle_post_upload(request);

	std::string path = FileHandler::decode_url(request.path); 
	if	(path == "/cgi-bin/script.py")
		return handle_get_cgi(request, *this, PYTHON);
	if	(path == "/cgi-bin/rust_program")
		return handle_get_cgi(request, *this, RUST);

	return handle_error(405);
}

Response Response::handle_post_submit(const Request& request)
{
	std::string body = "<html><head><meta charset=\"utf-8\"><style>"
		+ std::string("body { background-color: black; color: #13d019; font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }")
		+ std::string("div { text-align: center; }")
		+ std::string("h1 { color: #13d019; }")
		+ std::string("</style></head><body><div><h1>201 Form data received</h1><p>File '") + request.body + std::string("' uploaded successfully</p></div></body></html>");

	return response_body(200, body);
}

Response Response::handle_post_upload(const Request& request)
{
	std::string content_type = request.getHeader("Content-Type");
	std::string boundary = Utils::extract_boundary(content_type);

	if (boundary.empty())
		return handle_error(400);

	std::string file_name = FileHandler::get_filename_from_multipart(request.body);
	std::string file_content = FileHandler::get_file_content(request.body, boundary);

	if (file_content.empty())
		return handle_error(400);

	std::string upload_path = _config->root + "/uploads/" + file_name;
	if (!FileHandler::save_uploaded_file(upload_path, file_content))
		return handle_error(500);

	std::string body = "<html><head><meta charset=\"utf-8\"><style>"
		+ std::string("body { background-color: black; color: #13d019; font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }")
		+ std::string("div { text-align: center; }")
		+ std::string("h1 { color: #13d019; }")
		+ std::string("</style></head><body><div><h1>201 Created</h1><p>File '") + file_name + std::string("' uploaded successfully</p>")
		+ std::string("<div style=\"margin-top: 30px;\"><a href=\"/index.html\"><button type=\"button\" class=\"back-btn\">Back to Home</button></a></div>")
		+ std::string("</div></body></html>");

	Response response = response_body(201, body);
	response.set_header("Location", "/uploads/" + file_name);
	return response;
}

Response Response::handle_redirect()
{
	Response response; 

	response.set_status(301);
	response.set_header("Content-Type", "text/html; charset=UTF-8");
	response.set_header("Location", "/redirect.html");
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
	response.set_body("");
	return (response); 
}
Response Response::handle_delete(const Request &request)
{
	std::string file_name = FileHandler::decode_url(request.uri); 

	if (file_name.find("/uploads/") != 0)
	{
		LOG_ERROR("DELETE not allowed on: " << file_name);
		return handle_error(405);
	}
	if (!file_name.empty() && file_name[0] == '/')
		file_name = file_name.substr(1);
	if (file_name.find("uploads/") == 0)
		file_name = file_name.substr(8);
	if (file_name.empty())
	{
		LOG_ERROR("No filename provided");
		return handle_error(400);
	}

	std::string file_path = _config->root + "/uploads/" + file_name;
	int status_code = validate_file_writable(file_path);
	if (status_code != 0)
		return handle_error(status_code);
	if (std::remove(file_path.c_str()) != 0)
	{
		LOG_ERROR("File not found: " << file_path);
		return handle_error(Utils::get_errno_code());
	}

	std::string body = "<html><head><meta charset=\"utf-8\"><style>"
		+ std::string("body { background-color: black; color: #13d019; font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }")
		+ std::string("div { text-align: center; }")
		+ std::string("h1 { color: #13d019; }")
		+ std::string("</style></head><body><div><h1>200 Deleted</h1><p>File '") + file_name + std::string("' successfully deleted</p>")
		+ std::string("<div style=\"margin-top: 30px;\"><a href=\"/index.html\"><button type=\"button\" class=\"back-btn\">Back to Home</button></a></div>")
		+ std::string("</div></body></html>");
	return ((response_body(200, body)));
}

Response Response::handle_error(const int error_code)
{
	std::string error_file;

	switch(error_code)
	{
		case 400: error_file = "www/errors/400.html"; break;
		case 401: error_file = "www/errors/401.html"; break;
		case 403: error_file = "www/errors/403.html"; break;
		case 404: error_file = "www/errors/404.html"; break;
		case 405: error_file = "www/errors/405.html"; break;
		case 413: error_file = "www/errors/413.html"; break;
		case 500: error_file = "www/errors/500.html"; break;
		default: error_file= "www/errors/501.html";
	}

	std::string html_error_file = FileHandler::load_file(error_file); 
	return ((response_body(error_code, html_error_file)));
}

/* HELPER FUNCTION */
Response Response::handle_directory(const std::string &uri, std::string &file_path)
{
	std::string index_file = file_path + "/";
	if (!_config->index.empty()) {
		index_file  += _config->index[0];
	}
	if (FileHandler::file_exists(index_file))
	{
		file_path = index_file;
		return Response();
	}

	std::string autoindex_html = FileHandler::handle_autoindex(uri, file_path);
	if (autoindex_html.empty())
		return handle_error(Utils::get_errno_code());
	return response_body(200, autoindex_html);
}

std::string Response::file_path_check(const std::string &uri)
{
    std::string file_path = uri;
    
	if (!_config) {
		LOG_ERROR(file_path << "_config is null!");
		return "";
	}

	if (file_path.find("/uploads/") == 0) 
		file_path = _config->root + file_path;
	else if (file_path == "/") {
		if (_config->index.empty()) {
			LOG_WARNING("No index file configured, using default");
			return _config->root + "/index.html";
		}
		file_path = _config->root + "/" + _config->index[0];
	}
	else
		file_path = _config->root + "/base_page" + file_path;
		
	return file_path;
}

/* Sends response to socket (creates one liner) */
void Response::set_body(const std::string &html_body)
{
	_html_body = html_body;
	std::stringstream content_length_stream;
	content_length_stream << _html_body.size();
	_headers["Content-Length"] = content_length_stream.str();
}

/* Sends response to socket (creates one liner) */
std::string Response::serialize()
{
	std::ostringstream http_response;
	http_response << version << " " << _status_code << " " << reason_message(_status_code) << NEW_LINE;
	LOG_INFO(this->_method << " " << this->_request_uri << " " << version << " " << _status_code << " " << reason_message(_status_code));
	
	// write all headers
	for (std::map<std::string, std::string>::const_iterator map_item = this->_headers.begin();
			map_item != this->_headers.end();
			++map_item)
	{
		http_response << map_item->first << ": " << map_item->second << NEW_LINE;
	}

	http_response << NEW_LINE;
	http_response << this->_html_body;
	return http_response.str();
} 

/* Matches error code and returns error message  */
std::string Response::reason_message(int status_code)
{
	switch (status_code)
	{
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 301: return "Moved Permanently";
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

/* Returns response body information */
Response Response::response_body(const int &status_code, const std::string &body)
{
	Response response; 

	response.set_status(status_code);
	response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
	response.set_body(body);
	return (response); 
}

int Response::validate_file_path(const std::string& file_path)
{
	if (!FileHandler::file_exists(file_path)) {
		LOG_INFO("404 Not Found: " << file_path);
		return 404;
    }
    if (!FileHandler::is_readable(file_path)) {
        LOG_ERROR("File not readable: " << file_path);
        return Utils::get_errno_code();
    }
	struct stat file_info;
	if (stat(file_path.c_str(), &file_info) == 0)
	{
		if (static_cast<size_t>(file_info.st_size) > MAX_FILE_SIZE)
		{
			LOG_ERROR("File too large: " << file_path << file_info.st_size << " bytes, max: " << MAX_FILE_SIZE);
			return 413;
		}
	}
    return 0;
}

int Response::validate_file_writable(const std::string& file_path)
{
	if (!FileHandler::file_exists(file_path)) {
		LOG_ERROR("File doesn't exist: " << file_path);
		return 404;
	}

	if (!FileHandler::is_writable(file_path)) {
		LOG_ERROR("File not writable: " << file_path);
		return 403;
	}
	return 0;
}
