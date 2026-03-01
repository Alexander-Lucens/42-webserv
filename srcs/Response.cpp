# include "Response.hpp"
# include "Request.hpp"
# include "CGI.hpp"
# include "Logger.hpp"
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <ctime>
# include <sstream>

#define to_string(x) static_cast<std::ostringstream&>(std::ostringstream() << (x)).str()

std::string Response::generate_session_id() {
    std::stringstream ss;
    ss << std::time(NULL) << rand();
    return ss.str();
}

void Response::set_cookie(const std::string &name, const std::string &value, int max_age, const std::string &path) {
    std::string cookie = name + "=" + value;
    
    if (max_age >= 0) {
        std::stringstream ss;
        ss << max_age;
        cookie += "; Max-Age=" + ss.str();
    }
    
    cookie += "; Path=" + path;
    cookie += "; HttpOnly";
    
    _headers.insert(std::pair<std::string, std::string>("Set-Cookie", cookie));
}

void Response::set_session_cookie(const std::string &name, const std::string &value) {
    set_cookie(name, value, -1, "/");
}

// Default constructor
Response::Response() : _status_code(200), version("HTTP/1.1") {}

// Copy constructor
Response::Response(const Response& other)
	:	_status_code(other._status_code),
		_html_body(other._html_body),
		_headers(other._headers),
		_method(other._method),
		_request_uri(other._request_uri),
		_cookies(other._cookies),
      	version(other.version)  {}

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
		_cookies = other._cookies;
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
	this->_method = request.method;
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
	const_cast<Request&>(request).parseCookies();

	set_config(request);
	set_method(request);
	
	int error_code = validate_request_by_configuration(request);
	if (error_code != 0) {
		LOG_WARNING("Request validation failed for uri: " << request.uri 
			<< " method: " << request.method 
			<< " with code: " << error_code);
		return handle_error(error_code);
	}

	 _request_uri = request.path;

	/* Its going next, update to dynamic redirection*/
	if (_config->locations.find(_conf_location_path) != _config->locations.end()) {
        const LocationConfig& loc = _config->locations.at(_conf_location_path);
        if (loc.redirection.status_code > 0) {
            return handle_redirect(request);
        }
    }
	/*	========= END ============. */
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
    
    if (request.path.find(".py") != std::string::npos || request.path.find("python") != std::string::npos)
		return handle_get_cgi(request, *this, PYTHON);
    if (request.path == "/cgi-bin/rust_program")
        return handle_get_cgi(request, *this, RUST);

    Response response;
    std::string file_path = file_path_check(uri);

    int status_code = validate_file_path(file_path);
    if (status_code != 0)
        return handle_error(status_code);

    if (FileHandler::is_directory(file_path)) {
        LOG_DEBUG("Path is directory: " << file_path);
        
        if (_config->locations.count(_conf_location_path)) {
            const LocationConfig& loc = _config->locations.at(_conf_location_path);
            
            if (loc.upload_enabled && loc.autoindex) {
                std::string html = FileHandler::handle_autoindex(uri, file_path);
                response.set_status(200);
                response.set_header("Content-Type", "text/html; charset=UTF-8");
                response.set_header("Date", Utils::get_http_date());
                response.set_header("Server", SERVER);
                response._method = _method;
                response.set_body(html);
                return response;
            }
        }
        
        return handle_directory(uri, file_path);
    }

    std::string body = FileHandler::load_file(file_path);
    if (body.empty()) {
        LOG_ERROR("File not loading: " << file_path);
        return handle_error(500);
    }
    
    if (uri.find("/uploads/") == 0)
        response.set_download_header(uri);
    
    response.set_status(200);
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
    response.set_header("Content-Type", FileHandler::find_content_type(file_path));
    response.set_header("Content-Length", to_string(body.length()));
    
    response.set_body(body);
	response._method = _method;
    return response;
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

	if (_config->locations.count(_conf_location_path)) {
		std::string ext = _config->locations.at(_conf_location_path).cgi_ext;
		if (!ext.empty() && request.uri.find(ext) != std::string::npos) {
			return handle_post_cgi(request, *this, ext == ".py" ? PYTHON : RUST);
		}
	}
	
	return handle_error(405);
}

Response Response::handle_post_submit(const Request& request)
{
	std::string username = parse_form_data_value(request.body, "username");
    std::string password = parse_form_data_value(request.body, "password");
    
	LOG_INFO("Sign-in attempt for user: " << username);

    if (username.empty() || password != _config->serverPassword) {
		LOG_WARNING("Authentication failed for user: " << username);
		return handle_error(401);
	}

	ServerConfig* config = const_cast<ServerConfig*>(_config);
	
	std::string session_id = generate_session_id();
	config->active_sessions[session_id]["username"] = username;
	config->active_sessions[session_id]["submit_time"] = Utils::get_http_date();
	config->active_sessions[session_id]["authenticated"] = "true";

	LOG_INFO("User '" << username << "' authenticated successfully");

	std::string body = generate_success_page("Login Successful", 
        "Welcome, " + username + "! You are now authenticated.");
    
    Response response;
    response.set_status(200);
    response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
    response.set_session_cookie("session_id", session_id);
    response._method = _method;
    response.set_body(body);
    
    return response;
	
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

	std::string body = generate_success_page("201 Created", 
        "File '" + file_name + "' uploaded successfully.");
    
    Response response;
    response.set_status(201);
    response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
	if (request._cookies.count("session_id")) {
		std::string session_id = request._cookies.at("session_id");
		response.set_session_cookie("session_id", session_id);
	} 
    response._method = _method;
	response.set_header("Location", "/uploads/" + file_name);
    response.set_body(body);
    
    return response;
}

Response Response::handle_redirect(const Request& request)
{
    Response response;
    const LocationConfig& loc = _config->locations.at(_conf_location_path);

    response.set_status(loc.redirection.status_code);
    response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Location", loc.redirection.to);
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
    
    if (request._cookies.count("session_id")) {
        std::string session_id = request._cookies.at("session_id");
        response.set_session_cookie("session_id", session_id);
    }
    
    response._method = _method;
    response.set_body("");
    return response;
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

	std::string body = generate_success_page("200 Deleted", 
        "File '" + file_name + "' deleted successfully.");
    
    Response response;
    response.set_status(200);
    response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
	if (request._cookies.count("session_id")) {
		std::string session_id = request._cookies.at("session_id");
		response.set_session_cookie("session_id", session_id);
	} 
    response._method = _method;
	response.set_header("Location", "/uploads/" + file_name);
    response.set_body(body);
    
    return response;
}

Response Response::handle_error(int error_code)
{
    Response response;
    response.set_status(error_code);

    std::string body;
    std::string error_page_path = "";
    
    if (_config && _config->error_pages.count(error_code)) {
        error_page_path = _config->error_pages.at(error_code);
        std::string full_path = _config->root + error_page_path;
        
        if (FileHandler::file_exists(full_path)) {
            body = FileHandler::load_file(full_path);
            LOG_INFO("Loaded error page for " << error_code << " from: " << full_path);
        } else {
            body = generate_error_page(error_code);
        }
    } else {
        body = generate_error_page(error_code);
    }

    response.set_header("Content-Type", "text/html; charset=UTF-8");
    response.set_header("Date", Utils::get_http_date());
    response.set_header("Server", SERVER);
    response._method = _method;
    response.set_body(body);
    return response;
}

Response Response::handle_directory(const std::string &uri, std::string &file_path) {
    
    if (_config->locations.count(_conf_location_path)) {
        const LocationConfig& loc = _config->locations.at(_conf_location_path);
        if (loc.upload_enabled) {
            return handle_error(403);
        }
    }
    
    std::vector<std::string> indexes = _config->index;
    bool autoindex = false;
    
    if (_config->locations.count(_conf_location_path)) {
        const LocationConfig& loc = _config->locations.at(_conf_location_path);
        if (!loc.index.empty()) 
            indexes = loc.index;
        autoindex = loc.autoindex;
    }
    
    for (size_t i = 0; i < indexes.size(); ++i) {
        std::string idx_path = file_path;
        if (idx_path[idx_path.length() - 1] != '/') 
            idx_path += "/";
        idx_path += indexes[i];
        
        // LOG_DEBUG("Checking for index file: " << idx_path);
        
        if (FileHandler::file_exists(idx_path)) {
            std::string body = FileHandler::load_file(idx_path);
        
            Response response;
            response.set_status(200);
            response.set_header("Date", Utils::get_http_date());
            response.set_header("Server", SERVER);
            response.set_header("Content-Type", FileHandler::find_content_type(idx_path));
            response.set_header("Content-Length", to_string(body.size()));
            response._method = _method;
            response.set_body(body);
            return response;
        }
    }
    
	std::string full_directory_path = _config->root + uri;

    if (autoindex) {
        LOG_INFO("Autoindex enabled for: " << file_path);
        std::string autoindex_html = FileHandler::handle_autoindex(uri, full_directory_path);
        if (!autoindex_html.empty()) {
            Response response;
            response.set_status(200);
            response.set_header("Content-Type", "text/html; charset=UTF-8");
            response.set_header("Date", Utils::get_http_date());
            response.set_header("Server", SERVER);
            response._method = _method;
            response.set_body(autoindex_html);
            return response;
        }
    }
    
    LOG_WARNING("Directory access forbidden: " << file_path);
    return handle_error(403);
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
	LOG_INFO(this->_method << "" << this->_request_uri << " " << version << " " << _status_code << " " << reason_message(_status_code));

	
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
	response._method =  _method;
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

std::string Response::parse_form_data_value(const std::string &body, const std::string &key)
{
    std::string search = key + "=";
    size_t pos = body.find(search);
    
    if (pos == std::string::npos)
        return "";
    
    pos += search.length();
    size_t end = body.find('&', pos);
    if (end == std::string::npos)
        end = body.length();
    
    std::string value = body.substr(pos, end - pos);
    
    std::string decoded;
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] == '+') {
            decoded += ' ';
        } else if (value[i] == '%' && i + 2 < value.length()) {
            int hex_value = 0;
            std::sscanf(value.c_str() + i + 1, "%2x", &hex_value);
            decoded += static_cast<char>(hex_value);
            i += 2;
        } else {
            decoded += value[i];
        }
    }
    
    return decoded;
}

std::string Response::generate_error_page(int error_code)
{
    std::string code_str;
    std::stringstream ss;
    ss << error_code;
    code_str = ss.str();
    
    std::string title = reason_message(error_code);
    std::string html = get_html_header(title);
    
    html += std::string(
        "<main class=\"container\">\n"
        "<h1 class=\"header-error\">" + code_str + "</h1>\n"
        "<h2 class=\"body-error\">" + title + "</h2>\n"
        "<p class=\"subtitle\" style=\"margin-bottom: 30px;\">An error occurred while processing your request.</p>\n"
        "<div class=\"menu-grid\" style=\"max-width: 300px;\">\n"
        "<a href=\"/index.html\"><button type=\"button\" class=\"nav-btn\">Back to Home</button></a>\n"
    );
    
    if (error_code == 401) {
        html += std::string(
            "<a href=\"/submit/index.html\"><button type=\"button\" class=\"nav-btn\" style=\"margin-top: 10px;\">Go to Sign In</button></a>\n"
        );
    }
    
    html += "</div>\n</main>\n";
    html += get_html_footer();
    return html;
}
