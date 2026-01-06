```sh
/config        → configuration parsing
/network       → socets + poll/select
/http
  ├─ Request   → HTTP request parsing
  ├─ Response  → creating Response
  └─ Status    → HTTP code(200, 404, etc.)
/server
  ├─ Server    → core of server {}
  └─ Location  → location {}
/cgi           → launch CGI
```

## What to read:
### Sockets and poll/select:

```sh
man socket
```

```sh
man bind
```

```sh
man listen
```

```sh
man accept
```

```sh
man recv
```

```sh
man send
```

```sh
man poll
```

❗what is **non-blocking socket**

Why is not allowed to **read()** without **poll**

### CGI
```sh
man fork
```

```sh
man execve
```

```sh
man pipe
```

what is **CGI environment variables**