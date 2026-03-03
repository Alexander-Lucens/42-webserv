*This project has been created as part of the 42 curriculum by akuzmin, amargolo, lkramer.*

---

## Description
**Webserv** is a custom HTTP/1.1 web server written in C++98 that replicates the functionality of NGINX. The server uses non-blocking I/O with `epoll` for connection handling and supports multiple features including static file serving, CGI script execution, configuration file parsing, request routing, authentification and cookie handling.

### Project Goals
- Build a fully functional HTTP/1.1 server 
- Implement non-blocking I/O for handling concurrent connections
- Support location-based routing with dynamic configuration inheritance
- Execute CGI scripts
- Handle file uploads, deletions, submit requests, and directory listing
- Provide proper HTTP status codes and error handling
- Handle authentification and cookies

### Overview
This server demonstrates low-level system programming concepts including socket programming, I/O multiplexing, process management, and HTTP protocol implementation. It can serve static files, execute dynamic scripts, and handle multiple concurrent connections efficiently.

---

## Instructions

### Prerequisites

You only need **Docker** to ensure a consistent development environment (Linux/Debian) across all OSs (Mac, Windows, Linux).

* **[Install Docker Desktop](https://www.docker.com/products/docker-desktop/)**

### Compilation & Execution

#### Quick Start

1. **Launch the Environment**
   
   Start the Docker container with the configured environment:
   ```bash
   ./run.sh
   ```
   *This will build the image, start the container, and drop you into a Linux shell.*

2. **Build the Project**
   
   Inside the container:
   ```bash
   make
   ```

3. **Run the Server**
   
   Start the server with a configuration file:
   ```bash
   ./webserv config/default.conf
   ```

4. **Test in Browser**
   
   Open your browser on your host machine (outside Docker):
   - **URL:** `http://localhost:8080`

5. **Stop and Exit**
   
   Inside the Docker container:
   ```bash
   exit
   ```

---

## Usage Examples

### Curl Test Commands

#### Basic GET request
```bash
curl -v http://localhost:8080/
```

#### GET a specific file
```bash
curl -v http://localhost:8080/index.html
```

#### GET with custom headers
```bash
curl -v -H "Host: example.com" http://localhost:8080/
```

#### GET to trigger 404
```bash
curl -v http://localhost:8080/nonexistent.html
```

#### GET to trigger 403 (Forbidden)
```bash
curl -v http://localhost:8080/private
```

#### GET to trigger 405 
```bash
curl -v http://localhost:8080/dont-have-access
```

#### GET to trigger 400
```bash
curl -v http://localhost:8080/test%00.html
```
#### GET a file from uploads directory
```bash
curl -v http://localhost:8080/uploads/webserv
```

#### POST file upload (multipart/form-data)
```bash
curl -v -F "file=@webserv" http://localhost:8080/upload
```

#### DELETE specific
```bash
curl -v -X DELETE http://localhost:8080/uploads/filename.txt
```

#### Basic POST with form data
```bash
curl -X POST -d 'username=Lara&password=enterpassword' http://localhost:8080/submit
```


---

## Resources

### HTTP/1.1 Specification
- [RFC 7230 - HTTP/1.1 Message Syntax and Routing](https://tools.ietf.org/html/rfc7230)
- [RFC 7231 - HTTP/1.1 Semantics and Content](https://tools.ietf.org/html/rfc7231)

### NGINX Documentation
- [NGINX official documentation](https://nginx.org/en/docs/)
- [NGINX configuration directives](https://nginx.org/en/docs/dirindex.html)

### C++ and System Programming
- [C++ Reference](https://cppreference.com/)
- [Linux man pages - epoll](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [Linux man pages - socket](https://man7.org/linux/man-pages/man2/socket.2.html)

### CGI Standards
- [RFC 3875 - CGI v1.1 Specification](https://tools.ietf.org/html/rfc3875)

### HTTP Status Codes
- [List of HTTP Status Codes](https://en.wikipedia.org/wiki/List_of_HTTP_status_codes)

---

## AI Usage

### Tasks and Parts That Used AI Assistance

* Helped design the overall architecture
* Provided guidance on explanation of key concepts
* Used for refactoring code 
* Supported inline CSS styling for error pages and UI components
* Supported refractoring on serialization logic and autoindex
* Helped structure shell scripts for automated testing


---

## Known Limitations & Future Improvements
### Current Limitations
* HTTP/1.0 and HTTP/1.1 only (no HTTP/2 or HTTP/3 support)
* Single-threaded event loop (not multi-process/multi-threaded)
No SSL/TLS support
Limited authentication (basic session cookies only)
* No database integration
* No compression support (gzip, deflate)
* Limited WebSocket support

### Potential Improvements
HTTP/2 server push
SSL/TLS encryption
Persistent connections (Connection: keep-alive)
Advanced caching strategies
* Load balancing across multiple servers
* Rate limiting and DDoS protection


**Last Updated:** March 2026  
**Version:** 1.0.0