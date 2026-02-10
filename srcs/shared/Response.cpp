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
	 std::cout << "DEBUG handle_request: method=[" << request.method << "] uri=[" << request.uri << "]" << std::endl;
	set_config(request);
	if (request.method == "GET")
		return (handle_get(request));
	if (request.method == "POST")
		return (handle_post(request));
	if (request.method == "DELETE")
		return (handle_delete(request));
	return (handle_error(501));
}

Response Response::handle_get(const Request& request)
{
	Response response;

	std::string normalized_html_path = request.uri; // Utils::normalize_path(request.uri);
	std::string file_path;

	// Route /uploads/ to uploads directory
	if (normalized_html_path.find("/uploads") == 0) 
		file_path = "www" + normalized_html_path;
	else 
	{
		if (normalized_html_path == "/")
		{
			file_path = _config->root + "/" + _config->index[0];
		} else {
			file_path = _config->root + "/base_page" + normalized_html_path;
		}
	}
	
	std::cout << "DEBUG: normalized_html_path = [" << normalized_html_path << "]" << std::endl;
	std::cout << "DEBUG: file_path = [" << file_path << "]" << std::endl;

	if (!FileHandler::file_exists(file_path))
		return handle_error(404);

	// If it's a directory, list files
    if (FileHandler::is_directory(file_path))
	{
		std::string index_file = file_path + "/" + _config->index[0];
		if (FileHandler::file_exists(index_file))
			file_path = index_file;
		else 
		{
			std::string autoindex_html = FileHandler::handle_autoindex(normalized_html_path, file_path);
			if (autoindex_html.empty())
				return handle_error(403);
			return response_body(200, autoindex_html);
    	}
	}
	if (!FileHandler::is_readable(file_path))
		return handle_error(403);

	std::string html_content = FileHandler::load_file(file_path);
	if(html_content.empty())
		return (handle_error(404)); 

	response.set_status(200);
	response.set_header("Date", Utils::get_http_date());
	response.set_header("Server", SERVER);
	response.set_header("Content-Type", FileHandler::find_content_type(file_path));
	response.set_body(html_content);
	return (response);
}

Response Response::handle_post(const Request& request)
{
	std::cout << "DEBUG handle_post: Content-Length header = [" << request.getHeader("Content-Length") << "]" << std::endl;
    std::cout << "DEBUG handle_post: content-length header = [" << request.getHeader("content-length") << "]" << std::endl;
    std::cout << "DEBUG handle_post: body size = [" << request.body.size() << "]" << std::endl;
    std::cout << "DEBUG handle_post: body = [" << request.body.substr(0, 100) << "...]" << std::endl;
    
	if (!FileHandler::validate_content_length(request)) 
	{
		std::cout << "DEBUG: validate_content_length FAILED" << std::endl;
		return handle_error(400);
	}

	if (request.uri.find("/submit") == 0)
		return handle_post_submit(request);

	if (request.uri.find("/upload") == 0)
		return handle_post_upload(request);

	return handle_error(405);
}

Response Response::handle_post_submit(const Request& request)
{
	std::string body = "<html><head><meta charset=\"utf-8\"><style>"
		+ std::string("body { background-color: black; color: #13d019; font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }")
		+ std::string("div { text-align: center; }")
		+ std::string("h1 { color: #13d019; }")
		+ std::string("</style></head><body><div><h1>200 Form data received</h1><p>File '") + request.body + std::string("' uploaded successfully</p></div></body></html>");

	return response_body(200, body);
}

Response Response::handle_post_upload(const Request& request)
{
	std::string content_type = request.getHeader("Content-Type");
	std::cout << "DEBUG: Content-Type = [" << content_type << "]" << std::endl;

	std::string boundary = Utils::extract_boundary(content_type);
	std::cout << "DEBUG: extracted boundary = [" << boundary << "]" << std::endl;

	if (boundary.empty())
		return handle_error(400);

	std::string file_name = FileHandler::get_filename_from_multipart(request.body);
	std::cout << "DEBUG: file_name = [" << file_name << "]" << std::endl;

	std::string file_content = FileHandler::get_file_content(request.body, boundary);
	std::cout << "DEBUG: file_content length = [" << file_content.length() << "]" << std::endl;

	if (file_content.empty())
		return handle_error(400);

	if (!FileHandler::save_uploaded_file("www/uploads/" + file_name, file_content))
		return handle_error(500);

	std::string body = "<html><head><meta charset=\"utf-8\"><style>"
		+ std::string("body { background-color: black; color: #13d019; font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }")
		+ std::string("div { text-align: center; }")
		+ std::string("h1 { color: #13d019; }")
		+ std::string("</style></head><body><div><h1>200 Created</h1><p>File '") + file_name + std::string("' uploaded successfully</p></div></body></html>");

	Response response = response_body(201, body);
	response.set_header("Location", "/uploads/" + file_name);
	return response;
}

Response Response::handle_delete(const Request &request)
{
	std::string filename = request.uri.substr(9);
	std::cout << "DEBUG: request.body = [" << request.body << "]" << std::endl;
    std::cout << "DEBUG: extracted filename = [" << filename << "]" << std::endl;
    if (filename.empty())
		return handle_error(400);
		
	std::string file_path = "www/uploads/" + filename;
	if (!FileHandler::file_exists(file_path))
		return (handle_error(404));

	if (std::remove(file_path.c_str()) != 0)
		return (handle_error(500));

	std::string body = "<html><head><meta charset=\"utf-8\"><style>"
	+ std::string("body { background-color: black; color: #13d019; font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }")
	+ std::string("div { text-align: center; }")
	+ std::string("h1 { color: #13d019; }")
	+ std::string("</style></head><body><div><h1>200 Deleted</h1><p>File '") + filename + std::string("' successfully deleted</p></div></body></html>");
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


/* HELPER FUNCTION */
/* Sends response to socket (creates one liner) */
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
	std::ostringstream http_response;
	http_response << version << " " << _status_code << " " << reason_message(_status_code) << NEW_LINE;

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

/* Returns response body information */
Response Response::response_body(const int &error_code, const std::string &body)
{
	Response response; 

	response.set_status(error_code);
	response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
	response.set_body(body);
	return (response); 
}
