# Team Development Guide

## Quick Reference: Who Owns What

### Person 1: Network & I/O Foundation
**Your Files:**
- `includes/Buffer.hpp` / `srcs/Buffer.cpp`
- `includes/Socket.hpp` / `srcs/Socket.cpp`
- `includes/EventLoop.hpp` / `srcs/EventLoop.cpp`
- `includes/Connection.hpp` / `srcs/Connection.cpp`

**Your Tasks:**
1. ✅ Basic structure created
2. ⏳ Complete kqueue implementation (macOS)
3. ⏳ Complete epoll implementation (Linux)
4. ⏳ Add select() fallback
5. ⏳ Error handling and edge cases
6. ⏳ Connection timeout handling

**How to Test:**
```bash
# Test with netcat
echo "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
```

### Person 2: HTTP Protocol Layer
**Your Files:**
- `includes/HTTPParser.hpp` / `srcs/HTTPParser.cpp`
- `includes/HTTPSerializer.hpp` / `srcs/HTTPSerializer.cpp`
- `includes/HTTPConnectionState.hpp` / `srcs/HTTPConnectionState.cpp`

**Your Tasks:**
1. ✅ Basic structure created
2. ⏳ Handle chunked transfer encoding
3. ⏳ Implement full HTTP/1.1 features
4. ⏳ Handle partial reads properly
5. ⏳ Validate HTTP format strictly
6. ⏳ Implement proper keep-alive logic

**How to Test:**
```cpp
// Test parser with mock buffer
Buffer buffer;
buffer.append("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
Request request;
HTTPParser parser;
parser.parse(buffer, request);
```

### Person 3: Application Layer
**Your Files:**
- `includes/Handler.hpp`
- `includes/Router.hpp` / `srcs/Router.cpp`
- `includes/ServerConfig.hpp` / `srcs/ServerConfig.cpp`
- `includes/StaticFileHandler.hpp` / `srcs/StaticFileHandler.cpp`

**Your Tasks:**
1. ✅ Basic structure created
2. ⏳ Implement full config file parser (NGINX-style)
3. ⏳ Add CGI handler
4. ⏳ Add file upload handler
5. ⏳ Add DELETE method handler
6. ⏳ Add POST method handler
7. ⏳ Implement route pattern matching (regex support?)

**How to Test:**
```cpp
// Test router with mock request
Request request;
request.method = "GET";
request.uri = "/";
Router router;
Response response = router.route(request);
```

## Integration Points (Critical!)

### Person 1 → Person 2
**When bytes arrive:**
```cpp
// In Person 1's code
Buffer& buffer = conn->getReadBuffer();
ssize_t consumed = httpState->onDataReceived(buffer);
// Remove consumed bytes from buffer
```

### Person 2 → Person 3
**When request is complete:**
```cpp
// In main.cpp (integration code)
if (httpState->hasCompleteRequest()) {
    Request& req = httpState->getRequest();
    Response res = router.route(req);  // Person 3
    httpState->setResponse(res);        // Person 2
}
```

### Person 3 → Person 2
**When creating response:**
```cpp
// Person 3 creates Response object
Response response;
response.setStatus(200, "OK");
response.body = "<html>...</html>";
response.setHeader("Content-Type", "text/html");
// Person 2 serializes it automatically
```

## Common Classes (Shared)

### Request (`includes/Request.hpp`)
- Used by: Person 2 (parsing), Person 3 (routing)
- Person 2 fills it, Person 3 reads it

### Response (`includes/Response.hpp`)
- Used by: Person 2 (serialization), Person 3 (creation)
- Person 3 creates it, Person 2 serializes it

## Building the Project

```bash
make          # Build
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Rebuild
```

## Running the Server

```bash
./webserv [config_file]
# If no config file, uses defaults (port 8080, ./www directory)
```

## Development Tips

1. **Start Simple:** Each person should get their basic functionality working first
2. **Test Independently:** Use mocks/stubs to test your layer without others
3. **Communicate:** Discuss interface changes before implementing
4. **Integration:** Test together frequently, not just at the end
5. **Error Handling:** Don't let your layer crash - return errors gracefully

## File Structure

```
includes/
  Request.hpp          # Shared
  Response.hpp         # Shared
  Buffer.hpp           # Person 1
  Socket.hpp           # Person 1
  EventLoop.hpp        # Person 1
  Connection.hpp       # Person 1
  HTTPParser.hpp       # Person 2
  HTTPSerializer.hpp   # Person 2
  HTTPConnectionState.hpp  # Person 2
  Handler.hpp          # Person 3
  Router.hpp           # Person 3
  ServerConfig.hpp     # Person 3
  StaticFileHandler.hpp # Person 3

srcs/
  main.cpp             # Integration (all work together)
  [corresponding .cpp files for each .hpp]
```

## Next Steps

1. **Person 1:** Get basic server accepting connections and echoing data
2. **Person 2:** Get HTTP parser working with simple GET requests
3. **Person 3:** Get router working with static file handler
4. **All:** Integrate in main.cpp and test end-to-end
