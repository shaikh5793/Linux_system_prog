# Connected UDP Implementation

A UDP implementation demonstrating connected sockets, which provide a simplified API while maintaining UDP's connectionless semantics.

## Attribution

**Author:** Raghu Bharadwaj (raghu@techveda.org)  
**Copyright:** 2024 Techveda (www.techveda.org)  
**License:** MIT License

## Architecture

This implementation follows the **Connected UDP Sockets** pattern, featuring:

- **Connected UDP Client**: Uses `connect()` to associate with a specific server
- **Regular UDP Server**: Standard UDP server using `recvfrom()`/`sendto()`
- **Simplified API**: Client uses `send()`/`recv()` instead of `sendto()`/`recvfrom()`
- **Performance Benefits**: Address caching and enhanced error reporting

### Connected UDP Architecture

Connected UDP sockets provide a simplified API while maintaining UDP semantics:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Connected UDP Socket Architecture                    │
│                                                                         │
│  Regular UDP Client         vs         Connected UDP Client             │
│  ┌─────────────────────┐              ┌─────────────────────┐           │
│  │ socket(SOCK_DGRAM)  │              │ socket(SOCK_DGRAM)  │           │
│  │ sendto(server_addr) │              │ connect(server_addr)│           │
│  │ recvfrom(buffer)    │              │ send(data)          │           │
│  │ sendto(server_addr) │              │ recv(buffer)        │           │
│  │ recvfrom(buffer)    │              │ send(data)          │           │
│  │ ...                 │              │ recv(buffer)        │           │
│  └─────────────────────┘              └─────────────────────┘           │
│                                                                         │
│  Server Perspective (Unchanged):                                       │
│  ┌───────────────────────────────────────────────────────────────────┐ │
│  │ socket(SOCK_DGRAM)                                                │ │
│  │ bind(server_addr)                                                 │ │
│  │ recvfrom(buffer, &client_addr)  // Works with both client types  │ │
│  │ sendto(response, client_addr)   // Responds to any client        │ │
│  └───────────────────────────────────────────────────────────────────┘ │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Connected UDP Benefits

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     Connected UDP Advantages                           │
├─────────────────────────────────────────────────────────────────────────┤
│ 1. Simplified API:                                                     │
│    - Use send()/recv() instead of sendto()/recvfrom()                  │
│    - No need to specify destination address repeatedly                 │
│                                                                         │
│ 2. Better Error Reporting:                                             │
│    - ICMP errors delivered to connected socket                         │
│    - Connection-oriented error handling                                │
│                                                                         │
│ 3. Security Benefits:                                                  │
│    - Prevents receiving packets from other sources                     │
│    - Filters incoming packets by source address                       │
│                                                                         │
│ 4. Performance Optimization:                                           │
│    - Kernel caches destination address                                 │
│    - Reduced address lookup overhead                                   │
│                                                                         │
│ 5. Programming Convenience:                                            │
│    - Familiar TCP-like API                                             │
│    - Easier porting from TCP applications                              │
└─────────────────────────────────────────────────────────────────────────┘
```

### Connection Establishment Flow

```
Client Side (Connected UDP):
1. socket(AF_INET, SOCK_DGRAM, 0)
2. connect(sockfd, server_addr, addr_len)    // "Connect" to server
3. send(sockfd, data, len, 0)                // Simplified send
4. recv(sockfd, buffer, len, 0)              // Simplified receive

Server Side (Unchanged):
1. socket(AF_INET, SOCK_DGRAM, 0)
2. bind(sockfd, server_addr, addr_len)
3. recvfrom(sockfd, buffer, len, 0, &client_addr, &addr_len)
4. sendto(sockfd, response, len, 0, &client_addr, addr_len)
```

## Files

### Core Implementation
- `connected_server.c` - Standard UDP server implementation
- `connected_client.c` - Connected UDP client implementation
- `connected_server` - Compiled server binary
- `connected_client` - Compiled client binary

### Test Scripts
- `start_connected_server.sh` - Server startup script
- `test_connected_client.sh` - Interactive client test
- `connected_test.sh` - Automated test suite

## Quick Start

### 1. Compile the Programs
```bash
cd connected
make
```

### 2. Start the Server
```bash
./start_connected_server.sh
```

### 3. Test with a Client
```bash
# In another terminal
./test_connected_client.sh
./connected_client 127.0.0.1 8006
```

## Usage Examples

### Basic Usage
```bash
# Terminal 1: Start server
./connected_server 8006

# Terminal 2: Start client
./connected_client 127.0.0.1 8006

# Follow interactive prompts in client
```

### Advanced Usage
```bash
# Test different server addresses
./connected_client 192.168.1.100 8006

# Use different port
./connected_server 8007
./connected_client 127.0.0.1 8007
```

## Protocol Details

### Server Port
- **Default Port**: 8006 (configurable)
- **Protocol**: UDP

### Communication Flow
```
Client Side (Connected UDP):
1. socket(AF_INET, SOCK_DGRAM, 0)     -> Create UDP socket
2. connect(sockfd, server_addr, len)   -> "Connect" to server
3. send(sockfd, data, len, 0)          -> Send data (simplified)
4. recv(sockfd, buffer, len, 0)        -> Receive response (simplified)

Server Side (Standard UDP):
1. socket(AF_INET, SOCK_DGRAM, 0)     -> Create UDP socket
2. bind(sockfd, server_addr, len)      -> Bind to port
3. recvfrom(sockfd, buf, len, 0, &addr, &len) -> Receive from any client
4. sendto(sockfd, response, len, 0, &addr, len) -> Send response to client
```

## Implementation Features

### Connected UDP Client Features
- **Simplified API**: Use `send()`/`recv()` instead of `sendto()`/`recvfrom()`
- **Address Caching**: Kernel caches destination address for performance
- **Enhanced Error Reporting**: ICMP errors delivered to connected socket
- **Security Benefits**: Only receives packets from connected address
- **Interactive Mode**: Menu-driven interface for testing

### Server Features
- **Standard UDP Server**: Works with both connected and regular UDP clients
- **Multi-client Support**: Handles multiple clients simultaneously
- **Echo Service**: Echoes received messages back to sender
- **Client Information**: Displays client address and port information
- **Signal Handling**: Graceful shutdown on SIGINT

## Connected UDP Benefits

### Performance Advantages
- **Reduced Overhead**: No need to specify destination address repeatedly
- **Kernel Optimization**: Address lookup cached in kernel
- **Faster Sending**: Slightly faster than `sendto()` for multiple sends

### Security Benefits
- **Source Filtering**: Only receives packets from connected address
- **ICMP Error Delivery**: Connection-oriented error reporting
- **Attack Prevention**: Prevents receiving packets from other sources

### API Simplification
- **Familiar Interface**: TCP-like API for UDP communication
- **Easier Porting**: Simpler to port from TCP applications
- **Reduced Complexity**: No need to manage destination addresses

## Important Limitations

### Still Unreliable
- **No Delivery Guarantees**: UDP semantics unchanged
- **No Ordering**: Packets may arrive out of order
- **No Duplicate Protection**: Application must handle duplicates

### Connection Semantics
- **No Actual Connection**: No connection state maintained
- **No Handshake**: No connection establishment protocol
- **No Flow Control**: No built-in flow control mechanisms

### Scope Limitations
- **One-to-One**: Can only communicate with one peer at a time
- **No Broadcast**: Cannot send broadcasts when connected
- **Reconnection Required**: Must disconnect/reconnect for new destination

## Client Interactive Features

The connected client provides an interactive menu system:

```
Connected UDP Client Menu:
1. Send message to server
2. Send multiple messages
3. Test error conditions
4. Display connection info
5. Reconnect to server
6. Exit
```

### Menu Options
- **Send Message**: Send single message and receive response
- **Multiple Messages**: Send burst of messages for testing
- **Error Testing**: Test various error conditions
- **Connection Info**: Display current connection details
- **Reconnect**: Reconnect to server (useful for testing)
- **Exit**: Clean shutdown

## Network Configuration

### Default Settings
- **Server IP**: 127.0.0.1 (localhost)
- **Server Port**: 8006
- **Buffer Size**: 1024 bytes
- **Socket Type**: UDP (SOCK_DGRAM)

### Customization
```bash
# Different server IP/port
./06_connected_client 192.168.1.100 8007

# Start server on different port
./06_connected_server 8007
```

## Test Scripts

### `start_connected_server.sh`
- Starts the connected UDP server on port 8006
- Handles PID management and process cleanup
- Provides startup status and server information

### `test_connected_client.sh`
- Tests interactive connected UDP client
- Runs for 30 seconds showing client functionality
- Demonstrates connected UDP features and benefits

### `connected_test.sh`
- Automated test suite with pass/fail results
- Tests client connection and socket setup
- Verifies interactive menu system functionality

## Comparison with Regular UDP

### Regular UDP Client
```c
// Must specify destination for each send
sendto(sockfd, data, len, 0, (struct sockaddr*)&server_addr, addr_len);
recvfrom(sockfd, buffer, len, 0, (struct sockaddr*)&from_addr, &addr_len);
```

### Connected UDP Client
```c
// Connect once, then use simplified API
connect(sockfd, (struct sockaddr*)&server_addr, addr_len);
send(sockfd, data, len, 0);        // Simplified send
recv(sockfd, buffer, len, 0);      // Simplified receive
```

## Server Compatibility

The server implementation works with both:
- **Regular UDP Clients**: Using `sendto()`/`recvfrom()`
- **Connected UDP Clients**: Using `send()`/`recv()` after `connect()`

This demonstrates the transparency of connected UDP from the server perspective.


## Related Documentation

This implementation demonstrates key concepts in connected UDP socket programming:
- Connected UDP Sockets pattern architecture and benefits
- Performance considerations and limitations analysis
- Simplified API design while maintaining UDP semantics
