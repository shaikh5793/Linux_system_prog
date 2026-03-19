# UDP Discovery System

A UDP-based discovery system demonstrating client-server registration and service discovery using connectionless sockets.

## Attribution

**Author:** Raghu Bharadwaj (raghu@techveda.org)  
**Copyright:** 2024 Techveda (www.techveda.org)  
**License:** MIT License

## Architecture

This implementation follows the **Service Discovery** model, featuring:

- **Discovery Server**: Maintains a registry of services and provides discovery services
- **Service Clients**: Register services and query for available services
- **Service Registry**: Server keeps track of registered services
- **Discovery Protocol**: Announcement and on-demand query mechanisms

### Service Discovery Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     Service Discovery Communication Model               │
│                                                                         │
│   Discovery Server                           Client Discovery           │
│   ┌─────────────────┐                       ┌─────────────────┐         │
│   │ Service Registry│                       │ Service Cache   │         │
│   │ ┌─────────────┐ │                       │ ┌─────────────┐ │         │
│   │ │ Service A   │ │    Announcements      │ │ Service A   │ │         │
│   │ │ Service B   │ │◄─────────────────────►│ │ Service B   │ │         │
│   │ │ Service C   │ │   (Periodic Broadcast)│ │ Service C   │ │         │
│   │ └─────────────┘ │                       │ └─────────────┘ │         │
│   │                 │                       │                 │         │
│   │ Announcement    │    Discovery Query    │ Query Handler   │         │
│   │ Scheduler       │◄─────────────────────►│                 │         │
│   │                 │   (On-demand Request) │                 │         │
│   └─────────────────┘                       └─────────────────┘         │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

## Files

### Core Implementation
- `discovery_server.c` - Discovery server implementation
- `discovery_client.c` - Discovery client implementation
- `discovery_server` - Compiled server binary
- `discovery_client` - Compiled client binary

### Test Scripts
- `start_discovery_server.sh` - Server startup script
- `test_discovery_client.sh` - Single client test
- `discovery_test.sh` - Automated test suite

## Quick Start

### 1. Compile the Programs
```bash
cd discovery
make
```

### 2. Start the Discovery Server
```bash
./start_discovery_server.sh
```

### 3. Test with a Client
```bash
# In another terminal
./test_discovery_client.sh
```

## Usage Examples

### Manual Testing
```bash
# Terminal 1: Start server
./start_discovery_server.sh

# Terminal 2: Start client
./discovery_client 127.0.0.1 8001

# Terminal 3: Multiple clients
./discovery_client 127.0.0.1 8001
```

### Automated Testing
```bash
# Run all tests
./discovery_test.sh
```

## Protocol Details

### Server Port
- **Default Port**: 8001
- **Protocol**: UDP

### Message Format
```
Client Registration:
    register              -> Server adds client to registry
    unregister            -> Server removes client from registry

Server Responses:
    ACK                   -> Acknowledgment of registration
    registered            -> Confirmation of successful registration
```

### Discovery Flow
1. **Registration**: Client sends `register` message
2. **Acknowledgment**: Server responds with `registered`
3. **Maintenance**: Client maintains connection with server
4. **Unregistration**: Client sends `unregister` on shutdown

## Implementation Features

### Server Features
- **Client Registry**: Maintains list of registered clients
- **Service Discovery**: Provides client lookup capabilities
- **Connection Management**: Handles client registrations
- **Signal Handling**: Graceful shutdown on SIGINT

### Client Features
- **Auto-Registration**: Automatically registers on startup
- **Connection Maintenance**: Maintains connection with server
- **Timeout Handling**: Uses select() for non-blocking operations
- **Clean Shutdown**: Unregisters before exit
- **Signal Handling**: Graceful shutdown on SIGINT

## Test Scripts

### `start_discovery_server.sh`
- Starts the discovery server on port 8001
- Handles PID management and process cleanup
- Provides startup status and configuration info

### `test_discovery_client.sh`
- Tests single client connecting to server
- Runs for 15 seconds showing registration process
- Shows complete client lifecycle

### `discovery_test.sh`
- Automated test suite with pass/fail results
- Tests client registration and server communication
- Verifies connection handling and cleanup

## Network Configuration

### Default Settings
- **Server IP**: 127.0.0.1 (localhost)
- **Server Port**: 8001
- **Client Timeout**: 1 second select timeout

### Customization
```bash
# Use different server IP/port
./discovery_client 192.168.1.100 8001

# Start server on different port
./discovery_server 8002
```


## Related Documentation

This implementation demonstrates key concepts in UDP-based service discovery systems:
- Service Discovery pattern architecture and design principles
- Performance considerations and reliability analysis
- Service registration and announcement protocols over UDP
