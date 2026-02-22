# Webserv

A custom HTTP/1.1 server written in C++98 using non-blocking I/O (`epoll`).
This project replicates the core functionality of NGINX, including static file serving, CGI support, and multiple port listening.

---

#### Authors: 
 - [Alexander Kuzmin](https://github.com/Alexander-Lucens)
 - [Antoine Margolloff](https://github.com/AntoineMargo)
 - [Lara Krämer](https://github.com/LaraKraemer)

---

## Prerequisites

You only need **Docker** to ensure a consistent development environment (Linux/Debian) across all OSs (Mac, Windows, Linux).

* **[Install Docker Desktop](https://www.docker.com/products/docker-desktop/)**

## Quick Start

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

4. **Test**
Open your browser on your host machine (outside Docker):
* **URL:** `http://localhost:8080`

5. **Stop and exit**
inside the Docker container:
```bash
exit
```


### Curl Tests

# Basic GET request
curl http://localhost:8080/

# GET a specific file
curl http://localhost:8080/index.html

# GET with custom headers
curl -H "Host: example.com" http://localhost:8080/

# GET to trigger 404
curl http://localhost:8080/nonexistent.html

# GET to trigger 403 (forbidden)
curl http://localhost:8080/error-403.html

# GET to trigger 400 (bad request)
curl http://localhost:8080/error-400

# GET a file from uploads directory
curl http://localhost:8080/uploads/myfile.txt

# POST file upload (multipart/form-data)
curl -F "file=@file2.txt" http://localhost:8080/upload


# Delete a specific file
curl -X DELETE http://localhost:8080/test.txt

# Delete with verbose output
curl -v -X DELETE http://localhost:8080/test.txt


// OPTIONAL ONLY IF WE KEEP THE SUBMIT PART

# Basic POST with form data
curl -X POST -d "filename=test.txt" http://localhost:8080/submit

# Malformed request return 400
curl "http://localhost:8080/test%00.html"


# POST with JSON data
curl -X POST -H "Content-Type: application/json" \
  -d '{"name":"test"}' http://localhost:8080/submit
---