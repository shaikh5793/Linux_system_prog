# TCP Sockets for Local IPC

**Author:** Raghu Bharadwaj(raghu@techveda.org)  
**Organization:** (C)2024 Techveda(www.techveda.org)
**License:** MIT

## Overview
TCP sockets can be used for local IPC when processes need network-compatible communication or when preparing for distributed systems. This section demonstrates localhost TCP communication patterns.

## Examples in Recommended Learning Order

### 1. Basic TCP Local IPC
- `01_basic_server.c` - TCP server binding to localhost
- `01_basic_client.c` - TCP client connecting to localhost
- **Learning Focus**: TCP socket fundamentals, localhost binding
- **Key Concepts**: AF_INET, SOCK_STREAM, bind(), listen(), accept(), connect()

### 2. Multi-Client TCP Server
- `02_multi_server.c` - Server handling multiple concurrent clients with fork()
- `02_multi_client.c` - Client for multi-server testing
- **Learning Focus**: Concurrent client handling, process forking
- **Key Concepts**: fork(), signal handling, zombie processes, SIGCHLD

### 3. Threaded TCP Server
- `03_threaded_server.c` - Multi-threaded server using pthreads
- `03_threaded_client.c` - Client for threaded server testing
- **Learning Focus**: Thread-based concurrency, shared memory
- **Key Concepts**: pthread_create(), pthread_detach(), thread safety

### 4. Heartbeat TCP Server
- `04_heartbeat_server.c` - Connection monitoring with heartbeat mechanism
- `04_heartbeat_client.c` - Client with periodic heartbeat messages
- **Learning Focus**: Connection monitoring, timeout detection
- **Key Concepts**: keep-alive, heartbeat patterns, connection health

## Compilation
```bash
make all
```

## Usage Examples
```bash
# Basic TCP IPC
./01_basic_server 8080 &
./01_basic_client 127.0.0.1 8080 "Hello TCP"

# Multi-client server (fork-based)
./02_multi_server 8081 &
./02_multi_client 127.0.0.1 8081 "Client 1" &
./02_multi_client 127.0.0.1 8081 "Client 2" &

# Threaded server (pthread-based)
./03_threaded_server 8082 &
./03_threaded_client 127.0.0.1 8082
# (interactive client - type messages and 'quit' to exit)

# Heartbeat server (connection monitoring)
./04_heartbeat_server 8083 &
./04_heartbeat_client 127.0.0.1 8083
# (client sends heartbeat every 10 seconds, server monitors connections)
```

### Clean up servers
```bash
make clean
```

## Key Concepts
- **Network compatibility**: Same code works locally and remotely
- **Performance**: Higher latency than Unix sockets but more portable
- **Concurrency**: Fork-based vs thread-based multi-client handling
- **Monitoring**: Connection health and heartbeat mechanisms
