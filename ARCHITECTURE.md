# Webserv Architecture - Three-Person Split

This document describes the architecture of the webserv project, organized for a three-person development team.

## Overview

The project is split into three distinct layers, each owned by a different person:

1. **Person 1: Network & I/O Foundation** - Lowest layer
2. **Person 2: HTTP Protocol Layer** - Middle layer  
3. **Person 3: Application Layer** - Highest layer

## Layer Responsibilities

### Person 1: Network & I/O Foundation

**Files:**
- `includes/Buffer.hpp` / `srcs/Buffer.cpp` - Buffer management
- `includes/Socket.hpp` / `srcs/Socket.cpp` - Socket operations
- `includes/EventLoop.hpp` / `srcs/EventLoop.cpp` - Event loop (kqueue/epoll/select)
- `includes/Connection.hpp` / `srcs/Connection.cpp` - Connection lifecycle

**Responsibilities:**
- Socket setup (listening, accepting connections)
- Event loop implementation (epoll/kqueue wrapper)
- Basic connection lifecycle (accept, read, write, close)
- Buffer management for reading/writing
- Non-blocking I/O operations

**Key Interfaces:**
- `EventLoop::poll()` - Main event loop
- `Connection::read()` / `Connection::write()` - Non-blocking I/O
- `Buffer` - Data buffer for reading/writing

### Person 2: HTTP Protocol Layer

**Files:**
- `includes/HTTPParser.hpp` / `srcs/HTTPParser.cpp` - HTTP request parser
- `includes/HTTPSerializer.hpp` / `srcs/HTTPSerializer.cpp` - HTTP response serializer
- `includes/HTTPConnectionState.hpp` / `srcs/HTTPConnectionState.cpp` - Connection state machine

**Responsibilities:**
- HTTP request parsing (handles partial reads, validates format)
- HTTP response serialization
- Connection state machine (integrates with Person 1's I/O callbacks)
- Keep-alive and connection management

**Key Interfaces:**
- `HTTPParser::parse()` - Parse HTTP request from buffer (returns bytes consumed)
- `HTTPConnectionState::onDataReceived()` - Callback when bytes arrive
- `HTTPConnectionState::setResponse()` - Set response to send
- `HTTPConnectionState::hasCompleteRequest()` - Check if request is ready

### Person 3: Application Layer

**Files:**
- `includes/Handler.hpp` - Handler interface
- `includes/Router.hpp` / `srcs/Router.cpp` - Request routing
- `includes/ServerConfig.hpp` / `srcs/ServerConfig.cpp` - Configuration parsing
- `includes/StaticFileHandler.hpp` / `srcs/StaticFileHandler.cpp` - Example handler

**Responsibilities:**
- Request routing system (URL pattern matching, method dispatch)
- Handler interface/framework
- Configuration file parsing
- Example handlers and business logic

**Key Interfaces:**
- `Handler::handle()` - Process Request → Response
- `Router::route()` - Route request to appropriate handler
- `ServerConfig::loadFromFile()` - Load configuration

## Integration Points

### Person 1 → Person 2

**Interface:** `HTTPConnectionState::onDataReceived(Buffer& buffer)`

When Person 1 receives data on a connection:
1. Data is read into `Connection::getReadBuffer()`
2. Person 1 calls `HTTPConnectionState::onDataReceived(readBuffer)`
3. Person 2 parses the data and returns bytes consumed
4. Person 1 removes consumed bytes from buffer

**Example:**
```cpp
// In Person 1's code (main.cpp handleRead)
Buffer& readBuffer = conn->getReadBuffer();
ssize_t consumed = httpState->onDataReceived(readBuffer);
```

### Person 2 → Person 3

**Interface:** `Router::route(const Request& request) → Response`

When Person 2 has a complete request:
1. Person 2 calls `HTTPConnectionState::hasCompleteRequest()`
2. If true, Person 3 routes the request: `router.route(request)`
3. Person 3 returns a `Response` object
4. Person 2 sets the response: `httpState->setResponse(response)`

**Example:**
```cpp
// In main.cpp
if (httpState->hasCompleteRequest()) {
    Request& request = httpState->getRequest();
    Response response = router.route(request);
    httpState->setResponse(response);
}
```

### Person 3 → Person 2

**Interface:** `Response` object format

Person 3 creates `Response` objects with:
- `statusCode` and `statusMessage`
- `headers` map
- `body` string

Person 2 serializes these to HTTP wire format.

## Data Flow

```
Client → Person 1 (Socket/Connection) 
      → Person 1 (Buffer) 
      → Person 2 (HTTPParser) 
      → Person 2 (HTTPConnectionState) 
      → Person 3 (Router) 
      → Person 3 (Handler) 
      → Person 3 (Response) 
      → Person 2 (HTTPSerializer) 
      → Person 1 (Buffer) 
      → Person 1 (Connection::write()) 
      → Client
```

## Development Workflow

### Person 1 can start with:
- Implement `Socket`, `Buffer`, `Connection` classes
- Implement basic `EventLoop` (can use select() initially, upgrade to kqueue/epoll later)
- Test with simple echo server

### Person 2 can start with:
- Implement `HTTPParser` (can test with mock buffers)
- Implement `HTTPSerializer` (can test with Response objects)
- Implement `HTTPConnectionState` (integrates parser/serializer)

### Person 3 can start with:
- Design `Handler` interface
- Implement `Router` with simple pattern matching
- Implement `ServerConfig` parser
- Create example handlers (StaticFileHandler)

### Integration:
- `main.cpp` ties all three layers together
- Each person can develop/test independently using mock interfaces
- Integration happens in `main.cpp`'s `WebServer` class

## Testing Strategy

Each person can test their layer independently:

- **Person 1:** Test with `nc` (netcat) or `telnet`, send raw bytes
- **Person 2:** Test parser with pre-made buffer data, test serializer with Response objects
- **Person 3:** Test router/handlers with pre-made Request objects

## Key Design Decisions

1. **Non-blocking I/O:** All I/O operations are non-blocking, using `poll()`/`kqueue()`/`epoll()`
2. **Buffer-based:** Data flows through `Buffer` objects, allowing partial reads/writes
3. **State machine:** `HTTPConnectionState` manages HTTP connection lifecycle
4. **Handler pattern:** Person 3 uses handler interface for extensibility
5. **Separation of concerns:** Each layer only knows about the layer directly below it

## Next Steps

1. **Person 1:** Complete kqueue/epoll implementation, add error handling
2. **Person 2:** Handle chunked encoding, implement full HTTP/1.1 features
3. **Person 3:** Implement config file parser, add more handlers (CGI, upload, etc.)
