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

---