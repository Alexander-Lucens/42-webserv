# include "../includes/Response.hpp"
# include "../includes/Request.hpp"

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

Response Response::handle_get(const Request& request)
{
	(void)request;
	Response response;
	response.status_code = 200;
	response.headers["Date"] = getHttpDate();
	response.headers["Server"] = "webserv/1.0";
	response.headers["Content-Type"] = "text/html";
	response.html_body = "<html><body><h1>GET Request Successful</h1></body></html>";
	return (response);
}

Response Response::handle_post(const Request& request)
{
	(void)request;
	Response response;
	response.status_code = 201;
	response.headers["Date"] = getHttpDate();
	response.headers["Server"] = "webserv/1.0";
	response.headers["Content-Type"] = "text/html";
	response.html_body = "<html><body><h1>POST Request Successful - Resource Created</h1></body></html>";
	return (response);
}
Response Response::handle_delete(const Request& request)
{
	(void)request;
	Response response;
	response.status_code = 204;
	response.headers["Date"] = getHttpDate();
	response.headers["Server"] = "webserv/1.0";
	response.headers["Content-Type"] = "text/html";
	response.html_body = "";
	return (response);
}

Response Response::handle_error(const int error_code)
{
	Response response;
	response.status_code = error_code;
	response.headers["Date"] = getHttpDate();
	response.headers["Server"] = "webserv/1.0";
	response.headers["Content-Type"] = "text/html";
	switch(error_code)
	{
		case 400:
			response.html_body ="<html><body><h1>400 Bad Request</h1></body></html>";
			break;
		case 401:
			response.html_body ="<html><body><h1>401 Unauthorized </h1></body></html>";
			break;
		case 404:
			response.html_body ="<html><body><h1>404 Forbidden</h1></body></html>";
			break;
		case 501:
			response.html_body ="<html><body><h1>501 Not implemented</h1></body></html>";
			break;
		default:
			response.html_body ="<html><body><h1>Error</h1></body></html>";
	}
	return (response);
}

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

std::string Response::getHttpDate() 
{
	std::time_t now = std::time(NULL);
	std::tm* gmt = std::gmtime(&now);

	char buf[128];
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return std::string(buf);
}


std::string Response::serialize(const Response &response)
{
	std::ostringstream out;
	out << "HTTP/1.1 " << response.status_code << " " << reason_message(response.status_code) << "\r\n";

	// write all headers
	for (std::map<std::string, std::string>::const_iterator map_item = response.headers.begin();
	     map_item != response.headers.end();
	     ++map_item)
	{
		out << map_item->first << ": " << map_item->second << "\r\n";
	}

	out << "Content-Length: " << response.html_body.size() << "\r\n";
	out << "\r\n";
	out << response.html_body;
	return out.str();
}

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
