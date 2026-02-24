# Webserv Architecture Update To-Do List

## 1. Configuration Parser (`ConfigParser.cpp` / `ConfigParser.hpp`)
- [ ] **Tokenization:** Update the tokenizer to correctly parse and strip semicolons (`;`) at the end of directives.
- [ ] **Nginx Directives:** Rename custom directives to standard Nginx syntax (e.g., use `allow_methods` instead of `methods`). Add support for parsing `client_max_body_size`.
- [ ] **Context Inheritance:** Implement inheritance so directives like `root`, `index`, `error_page`, and `client_max_body_size` defined in the `server` block automatically apply to `location` blocks unless overridden.
- [ ] **Error Pages:** Update `error_page` parsing to accept Nginx-style URIs (e.g., `/errors/404.html`) instead of strictly local relative paths.

## 2. Routing and Paths (`Response.cpp` / `Server.cpp`)
- [ ] **URI Resolution:** Update the path resolution logic to match the requested URI against the longest `location` prefix, and correctly swap/append the `root` path.
- [ ] **Static Files Routing:** Ensure the server correctly handles requests to the `/statics` directory so styles and scripts load without 404 errors.
- [ ] **Index Directive:** Implement logic to sequentially check for files listed in the `index` directive when a directory path (like `/` or `/upload/`) is requested.

## 3. MIME Types (`FileHandler.cpp` / `Utils.cpp`)
- [ ] **Content-Type Headers:** Update the MIME type dictionary to strictly assign `text/css` for `.css` files and `application/javascript` for `.js` files. (Crucial for frontend rendering).

## 4. CGI and Request Limits (`CGI.cpp` / `Request.cpp`)
- [ ] **CGI Routing Adaptation:** Trigger CGI execution based on file extensions (e.g., `cgi_ext .py`) and interpreter paths (`cgi_path`), rather than relying solely on the `/cgi-bin` directory path.
- [ ] **Payload Limits (Early Rejection):** Read `client_max_body_size` during request header parsing. Immediately return a `413 Payload Too Large` error if the `Content-Length` header exceeds the limit to protect the server during file uploads.