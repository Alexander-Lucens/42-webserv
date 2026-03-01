## Mandatory Part

* **Launch the installation of siege:** `brew install siege`
* **Explain the basics of an HTTP server:** An HTTP server listens on a TCP port for incoming client connections, parses the HTTP requests (Method, URI, Headers), processes the requested resource (static file or CGI), and returns an HTTP response with a status code and payload.
* **Which function is used for I/O Multiplexing:** The server uses `epoll` (`epoll_wait`, `epoll_ctl`, `epoll_create1`).
* **Explanation of how select (or equivalent) is working:** `epoll` registers multiple file descriptors (sockets) with the OS kernel. `epoll_wait` suspends the process until the kernel reports that one or more descriptors are ready for I/O operations, avoiding blocking or the need for multithreading.
* **Use only one select (epoll) and server accept / client read/write:** Yes, only one `epoll_fd` is used in the main `EventLoop::run()` loop. Server sockets trigger `accept()` in a non-blocking loop, while client sockets trigger `on_readable()` or `on_writable()`.
* **Main loop checks fd for read and write AT THE SAME TIME:** Yes, when a client connects, its FD is added to `epoll` with `EPOLLIN | EPOLLOUT | EPOLLET` simultaneously.
* **Only one read or one write per client per select:** In `EventLoop::run()`, if `EPOLLIN` is set, it calls `conn->on_readable()`. If `EPOLLOUT` is set, it calls `conn->on_writable()`. These methods execute a single reading/writing cycle per event without blocking.
* **Check if error returned, the client is removed & checking both -1 and 0:** In `Connection::on_readable()`, reading `0` (EOF) returns `false` to close the connection. Reading `-1` (if not EAGAIN/EWOULDBLOCK) sets the state to `ERROR` and returns `false` to remove the client.
* **Check of errno is done after read/write:** `errno` is explicitly checked for `EAGAIN` and `EWOULDBLOCK` after `read()` and `write()` to handle non-blocking socket behavior properly.
* **Writing/reading ANY fd without going through select is FORBIDDEN:** This rule is strictly followed by ensuring all socket I/O is non-blocking and driven by `epoll` events.
* **Project compilation without re-link:** Handled correctly via the `Makefile` rules.

## Configuration

* **HTTP response status codes:** The server returns appropriate HTTP 1.1 status codes (e.g., 200, 400, 403, 404, 405, 500) during evaluations.
* **Setup multiple servers with different port:** The `ConfigParser` parses `listen` directives and `Server::start()` creates unique sockets for different ports.
* **Setup multiple servers with different hostname:** Supported via the `server_name` directive; `ConfigParser::get_config()` matches the requested Host header to route the request to the correct server block.
* **Setup default error page:** The `error_page` directive maps specific error status codes to custom HTML file paths.
* **Limit the client body:** Configured via the `client_max_body_size` directive, which parses bytes and units (k, m, g) to restrict payload sizes.
* **Setup routes in a server to different directories:** Handled via `location /path { root /dir; }` blocks to map URI routes to specific local directories.
* **Setup a default file to search for directory:** Configured via the `index` directive inside server or location blocks.
* **Setup a list of methods accepted for a route:** Defined using the `limit_except` directive inside a location block to restrict HTTP methods (e.g., denying `DELETE` without permission).