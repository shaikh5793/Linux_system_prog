# UDP Event Notification System

A UDP-based event notification system demonstrating the publish-subscribe pattern using connectionless sockets.

## Attribution

**Author:** Raghu Bharadwaj (raghu@techveda.org)  
**Copyright:** 2024 Techveda (www.techveda.org)  
**License:** MIT License

## Architecture

This implementation follows the **Event Notification System** pattern, featuring:

- **Event Server**: Maintains a registry of subscribed clients and broadcasts events
- **Event Clients**: Register for events and receive notifications
- **Selective Delivery**: Only registered clients receive event notifications
- **Periodic Events**: Server generates test events for demonstration

### Event Notification Architecture

Event-driven UDP communication with client registration and selective notification:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Event Notification Architecture                      │
│                                                                         │
│   Event Server                              Registered Clients         │
│   ┌─────────────┐                           ┌─────────────────────┐     │
│   │             │     Registration          │                     │     │
│   │ Client      │◀─── "REGISTER:type" ──────│   Client A          │     │
│   │ Registry    │                           │   (event_type_1)    │     │
│   │             │                           │                     │     │
│   │ Event       │     Event Notification    │                     │     │
│   │ Queue       │──── "EVENT:data" ────────▶│   Client B          │     │
│   │             │                           │   (event_type_2)    │     │
│   │ Notification│                           │                     │     │
│   │ Logic       │                           │   Client C          │     │
│   │             │                           │   (event_type_1,2)  │     │
│   └─────────────┘                           └─────────────────────┘     │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Event Flow Sequence

```
Registration Phase:
1. Client sends REGISTER message with event types
2. Server adds client to registry with event subscriptions
3. Server sends REGISTERED confirmation

Event Notification Phase:
1. Server receives/generates event
2. Server looks up subscribed clients for event type
3. Server sends EVENT notification to each subscribed client
4. Clients process event notifications independently

Unregistration Phase:
1. Client sends UNREGISTER message
2. Server removes client from registry
3. Server sends UNREGISTERED confirmation
```

### Event Message Format

```
REGISTER:<event_type_1>,<event_type_2>,...
EVENT:<event_type>:<event_data>
UNREGISTER
REGISTERED:OK
UNREGISTERED:OK
```

## Files

### Core Implementation
- `event_server.c` - Event notification server implementation
- `event_client.c` - Event notification client implementation
- `event_server` - Compiled server binary
- `event_client` - Compiled client binary

### Test Scripts
- `start_event_server.sh` - Server startup script
- `test_event_client.sh` - Single client test
- `event_multi_client.sh` - Multiple clients test
- `event_test.sh` - Automated test suite

## Quick Start

### 1. Compile the Programs
```bash
cd /home/raghub/raghu/lpe/sockets/netp3
make
```

### 2. Start the Event Server
```bash
cd event
./start_event_server.sh
```

### 3. Test with a Client
```bash
# In another terminal
cd event
./test_event_client.sh
```

## Usage Examples

### Manual Testing
```bash
# Terminal 1: Start server
./start_event_server.sh

# Terminal 2: Start client
./event_client 127.0.0.1 8002

# Terminal 3: Multiple clients
./event_multi_client.sh
```

### Automated Testing
```bash
# Run all tests
./event_test.sh
```

## Protocol Details

### Server Port
- **Default Port**: 8002
- **Protocol**: UDP

### Message Format
```
Client Registration:
    register              -> Server adds client to registry
    unregister            -> Server removes client from registry

Event Broadcasts:
    event:type:data       -> Server broadcasts to all registered clients
```

### Event Flow
1. **Registration**: Client sends `register` message
2. **Acknowledgment**: Server responds with `registered`
3. **Event Generation**: Server generates periodic test events
4. **Event Broadcasting**: Server sends `event:system:data` to all clients
5. **Event Processing**: Clients receive and display events
6. **Unregistration**: Client sends `unregister` on shutdown

## Implementation Features

### Server Features
- **Client Registry**: Maintains list of registered clients
- **Event Broadcasting**: Sends events to all registered clients
- **Periodic Events**: Generates test events every 5 seconds
- **Client Management**: Handles registration/unregistration
- **Signal Handling**: Graceful shutdown on SIGINT

### Client Features
- **Auto-Registration**: Automatically registers on startup
- **Event Processing**: Parses and displays received events
- **Timeout Handling**: Uses select() for non-blocking operations
- **Clean Shutdown**: Unregisters before exit
- **Signal Handling**: Graceful shutdown on SIGINT

## Test Scripts

### `start_event_server.sh`
- Starts the event server on port 8002
- Handles PID management and process cleanup
- Provides startup status and configuration info

### `test_event_client.sh`
- Tests single client connecting to server
- Runs for 20 seconds showing event reception
- Shows complete client lifecycle

### `event_multi_client.sh`
- Tests multiple clients (3 by default)
- Shows broadcast nature of event notifications
- Runs for 15 seconds with proper cleanup

### `event_test.sh`
- Automated test suite with pass/fail results
- Tests client registration and event reception
- Verifies multiple event handling and cleanup

## Network Configuration

### Default Settings
- **Server IP**: 127.0.0.1 (localhost)
- **Server Port**: 8002
- **Client Timeout**: 1 second select timeout
- **Event Interval**: 5 seconds

### Customization
```bash
# Use different server IP/port
./event_client 192.168.1.100 8002

# Start server on different port
./event_server 8003
```


## Related Documentation

This implementation demonstrates key concepts in UDP-based event notification systems:
- Event Notification System pattern architecture and design
- Performance considerations and reliability analysis
- Publish-subscribe communication patterns over UDP
