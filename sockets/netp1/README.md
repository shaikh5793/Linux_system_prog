# Part 1: Unix Domain Sockets (AF_UNIX)

**Author:** Raghu Bharadwaj(raghu@techveda.org)
**Organization:** (C)2024 Techveda (www.techveda.org) 
**License:** MIT

## Overview
Unix domain sockets provide efficient IPC between processes on the same machine. They offer better performance than network sockets for local communication and support advanced features like credential passing.

## Examples in Recommended Learning Order

### 1. Basic Stream Communication (SOCK_STREAM)
- `01_stream_server.c` - Simple stream server using filesystem-based socket
- `01_stream_client.c` - Basic stream client with connection-oriented communication
- **Learning Focus**: Socket fundamentals, connection establishment, basic I/O
- **Key Concepts**: AF_UNIX, SOCK_STREAM, bind(), listen(), accept(), connect()

### 2. Datagram Communication (SOCK_DGRAM)
- `02_dgram_server.c` - Connectionless server using SOCK_DGRAM
- `02_dgram_client.c` - Datagram client with message boundaries
- **Learning Focus**: Connectionless communication, message preservation
- **Key Concepts**: sendto(), recvfrom(), no connection state, address binding

### 3. Interactive Stream Communication
- `03_interactive_server.c` - Multi-message server with session handling
- `03_interactive_client.c` - Interactive client with persistent connections
- **Learning Focus**: Session management, multiple message exchanges
- **Key Concepts**: Connection persistence, interactive I/O, proper cleanup

### 4. Abstract Namespace (Linux-Specific)
- `04_abstract_server.c` - Server using abstract namespace addressing
- `04_abstract_client.c` - Client for filesystem-independent sockets
- **Learning Focus**: Advanced addressing, container-friendly sockets
- **Key Concepts**: Abstract namespace, no filesystem entries, automatic cleanup

### 5. Credential Passing (Security Feature)
- `05_cred_server.c` - Server with process authentication
- `05_cred_client.c` - Client sending process credentials
- **Learning Focus**: Process authentication, security, ancillary data
- **Key Concepts**: SCM_CREDENTIALS, SO_PASSCRED, sendmsg(), recvmsg()

## Compilation
```bash
make all
```

## Usage Examples

### 1. Basic Stream Communication
```bash
# Terminal 1 - Start stream server
./01_stream_server

# Terminal 2 - Run stream client
./01_stream_client "Hello from client"
```

### 2. Datagram Communication
```bash
# Terminal 1 - Start datagram server
./02_dgram_server

# Terminal 2 - Run datagram client
./02_dgram_client "Hello datagram server"
```

### 3. Interactive Stream Communication
```bash
# Terminal 1 - Start interactive server
./03_interactive_server

# Terminal 2 - Run interactive client
./03_interactive_client "Initial message"
# Then type messages interactively
```

### 4. Abstract Namespace (Linux)
```bash
# Terminal 1 - Start abstract server
./04_abstract_server

# Terminal 2 - Run abstract client
./04_abstract_client "Hello abstract server"
# No filesystem cleanup needed!
```

### 5. Credential Passing
```bash
# Terminal 1 - Start credential server
./05_cred_server

# Terminal 2 - Run credential client
./05_cred_client "Hello with credentials"
# Server will display and verify client's PID, UID, GID
```

### Clean up socket files
```bash
make clean
```

## Key Concepts
- **Performance**: Faster than network sockets (no protocol overhead)
- **Security**: Filesystem permissions control access
- **Reliability**: Stream sockets guarantee ordered delivery
- **Features**: Support credential passing and file descriptor sharing
