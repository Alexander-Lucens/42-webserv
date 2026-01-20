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
Response::Response() : _status_code(200) {}

// Copy constructor
Response::Response(const Response& other)
	:	_status_code(other._status_code),
		_html_body(other._html_body),
		_headers(other._headers) {}


// Copy assignment operator
Response& Response::operator=(const Response& other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
		_html_body = other._html_body;
		_headers = other._headers;
	}
	return (*this);
}

// No custom containers that need explicit destruction
Response::~Response() {}


Response Response::handle_get(const Request& request)
{
	Response response;

	std::string html_path = request.path;
	size_t url_index = html_path.find("?");
	if (url_index != std::string::npos)
		html_path = html_path.substr(0, url_index);

	if (html_path == "/" || html_path.empty())
		html_path = "/index.html";
	std::string file_path = "www/base_page" + html_path;
	// Open file
	std::ifstream file(file_path.c_str(), std::ios::binary);
	if(!file.is_open())
		return (handle_error(404));
	
	// Read file
	std::stringstream buffer;
	buffer << file.rdbuf();
	response._html_body = buffer.str();
	file.close();

	response.set_status(200);
	response.set_header("Date", getHttpDate());
	response.set_header("Server", SERVER);
	response.set_header("Content-Type", "text/html");
	
	// response.html_body = "<html><body><h1>GET Request Successful</h1></body></html>";
	return (response);
}

Response Response::handle_post(const Request& request)
{
	(void)request;
	Response response;
	response.set_status(201);
	response.set_header("Date", getHttpDate());
	response.set_header("Server", SERVER);
	response.set_header("Content-Type", "text/html");
	
	// response.html_body = "<html><body><h1>POST Request Successful - Resource Created</h1></body></html>";
	return (response);
}
Response Response::handle_delete(const Request& request)
{
	(void)request;
	Response response;
	response.set_status(204);
	response.set_header("Date", getHttpDate());
	response.set_header("Server", SERVER);
	response.set_header("Content-Type", "text/html");
	
	// response.html_body = "";
	return (response);
}

Response Response::handle_error(const int error_code)
{
	Response response;
	response.set_status(200);
	response.set_header("Date", getHttpDate());
	response.set_header("Server", SERVER);
	response.set_header("Content-Type", "text/html");
	
	std::string error_file;
	switch(error_code)
	{
		case 400:
			error_file = "www/errors/400.html";
			break;
		case 401:
			error_file = "www/errors/401.html";
			break;
		case 404:
			error_file = "www/errors/404.html";
			break;
		case 500:
			error_file = "www/errors/500.html";
			break;
		default:
			error_file= "www/errors/501.html";
	}

	// Read file
	std::ifstream file(error_file.c_str(), std::ios::binary);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		response._html_body = buffer.str();
		file.close();
	}
	return (response);
}

/* Filters request type and calls  */
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
std::string Response::getHttpDate() 
{
	std::time_t now = std::time(NULL);
	std::tm* gmt = std::gmtime(&now);

	char buffer[128];
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return (std::string(buffer));
}

void Response::set_status(int status_code)
{
	_status_code = status_code;
}

void Response::set_header(const std::string &key, const std::string &value)
{	
	_headers[key] = value;
}

void Response::set_body(const std::string &html_body)
{
	_html_body = html_body;
	_headers["Content-Length"] = html_body.size();
}

/* Sends response to socket (creates one liner) */
std::string Response::serialize()
{
	std::ostringstream out;
	out << "HTTP/1.1 " << this->_status_code << " " << reason_message(this->_status_code) << NEW_LINE;

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
