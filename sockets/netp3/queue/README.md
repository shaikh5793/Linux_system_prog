# UDP Message Queue System

A UDP-based message queue system implementing the producer-consumer pattern using connectionless sockets.

## Attribution

**Author:** Raghu Bharadwaj (raghu@techveda.org)  
**Copyright:** 2024 Techveda (www.techveda.org)  
**License:** MIT License

## Architecture

This implementation follows the **Message Queue Pattern**, featuring:

- **Queue Server**: Manages FIFO message queues with persistence
- **Queue Client**: Single binary that can act as producer, consumer, or monitor
- **Producer Role**: Client using `send` operation to add messages
- **Consumer Role**: Client using `get` operation to retrieve messages
- **Monitor Role**: Client using `status` operation for queue monitoring

### Message Queue Architecture

The Message Queue Pattern implements UDP-based producer-consumer communication:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      Message Queue Architecture                         │
│                                                                         │
│   Producers                Queue Server               Consumers         │
│                                                                         │
│  ┌─────────────┐           ┌─────────────┐           ┌─────────────┐     │
│  │ Producer A  │  SEND     │   Message   │  RECEIVE  │ Consumer X  │     │
│  │             │──────────▶│   Queue     │◀──────────│             │     │
│  └─────────────┘           │             │           └─────────────┘     │
│                            │ ┌─────────┐ │                               │
│  ┌─────────────┐           │ │ Queue 1 │ │           ┌─────────────┐     │
│  │ Producer B  │  SEND     │ │ Queue 2 │ │  RECEIVE  │ Consumer Y  │     │
│  │             │──────────▶│ │ Queue 3 │ │◀──────────│             │     │
│  └─────────────┘           │ │   ...   │ │           └─────────────┘     │
│                            │ └─────────┘ │                               │
│  ┌─────────────┐           │             │           ┌─────────────┐     │
│  │ Producer C  │  SEND     │ Message     │  RECEIVE  │ Consumer Z  │     │
│  │             │──────────▶│ Routing     │◀──────────│             │     │
│  └─────────────┘           │ Logic       │           └─────────────┘     │
│                            └─────────────┘                               │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Queue Flow Sequence

```
Producer Operations:
1. send:message_data → Server queues message
2. Server responds with OK (success) or FULL (queue full)

Consumer Operations:
1. get → Server dequeues message from queue
2. Server responds with msg:content (success) or EMPTY (no messages)

Queue Management:
1. status → Returns current queue utilization (count/max)
```

### Implementation Reality

Our implementation uses a **single `queue_client` binary** that can fulfill different roles:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Actual Implementation Architecture                    │
│                                                                         │
│   queue_client          Queue Server            queue_client            │
│   (Producer Role)                                (Consumer Role)         │
│                                                                         │
│  ┌─────────────┐           ┌─────────────┐           ┌─────────────┐     │
│  │ Terminal 1  │  send:msg │   Message   │  get      │ Terminal 2  │     │
│  │ ./client    │──────────▶│   Queue     │◀──────────│ ./client    │     │
│  │ send "msg" │           │   (FIFO)    │           │ get         │     │
│  └─────────────┘           │             │           └─────────────┘     │
│                            │             │                               │
│  ┌─────────────┐           │             │           ┌─────────────┐     │
│  │ Terminal 3  │  status   │             │  status   │ Terminal 4  │     │
│  │ ./client    │──────────▶│             │◀──────────│ ./client    │     │
│  │ status      │           │             │           │ status      │     │
│  └─────────────┘           └─────────────┘           └─────────────┘     │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

**Key Point**: The same `queue_client` binary performs different roles based on the operation:
- **Producer**: `./queue_client server port send "message"`
- **Consumer**: `./queue_client server port get`
- **Monitor**: `./queue_client server port status`

## Files

### Core Implementation
- `queue_server.c` - Message queue server implementation
- `queue_client.c` - Message queue client implementation
- `queue_server` - Compiled server binary
- `queue_client` - Compiled client binary

### Test Scripts
- `start_queue_server.sh` - Server startup script
- `test_queue_client.sh` - Queue operations test

## Quick Start

### 1. Compile the Programs
```bash
cd /home/raghub/raghu/lpe/sockets/netp3
make
```

### 2. Start the Queue Server
```bash
cd queue
./start_queue_server.sh
```

### 3. Test with a Client
```bash
# In another terminal
cd queue
./test_queue_client.sh
```

## Usage Examples

### Manual Testing
```bash
# Terminal 1: Start server
./start_queue_server.sh

# Terminal 2: Send messages
./queue_client 127.0.0.1 8003 send "Hello World"
./queue_client 127.0.0.1 8003 send "Message 2"

# Terminal 3: Receive messages
./queue_client 127.0.0.1 8003 get
./queue_client 127.0.0.1 8003 get

# Check queue status
./queue_client 127.0.0.1 8003 status
```

### Automated Testing
```bash
# Test queue operations
./test_queue_client.sh
```

### Producer-Consumer Example
```bash
# Terminal 1: Start the queue server
./start_queue_server.sh

# Terminal 2: Act as PRODUCER - send messages
./queue_client 127.0.0.1 8003 send "First message"
./queue_client 127.0.0.1 8003 send "Second message"  
./queue_client 127.0.0.1 8003 send "Third message"

# Terminal 3: Act as CONSUMER - retrieve messages
./queue_client 127.0.0.1 8003 get    # Gets "First message"
./queue_client 127.0.0.1 8003 get    # Gets "Second message"
./queue_client 127.0.0.1 8003 get    # Gets "Third message"

# Terminal 4: Act as MONITOR - check status
./queue_client 127.0.0.1 8003 status # Shows current queue state
```

## Protocol Details

### Server Port
- **Default Port**: 8003
- **Protocol**: UDP

### Message Format
```
Producer Operations:
    send:message          -> Server queues message, responds with OK/FULL

Consumer Operations:
    get                   -> Server dequeues message, responds with msg:content/EMPTY

Queue Management:
    status                -> Server responds with count/max (e.g., "3/10")
```

### Response Format
```
Server Responses:
    OK                    -> Message successfully queued
    FULL                  -> Queue is full, message rejected
    msg:content           -> Message retrieved from queue
    EMPTY                 -> No messages in queue
    count/max             -> Queue status (e.g., "0/10", "5/10")
    ERROR                 -> Unknown command
```

## Implementation Features

### Server Features
- **FIFO Queue**: First-in-first-out message ordering
- **Message Persistence**: Messages stored until consumed
- **Queue Limits**: Configurable maximum queue size (10 messages)
- **Status Monitoring**: Real-time queue count tracking
- **Error Handling**: Proper response for invalid operations
- **Signal Handling**: Graceful shutdown on SIGINT

### Client Features
- **Multi-Role Design**: Single binary acts as producer, consumer, or monitor
- **Multiple Operations**: Send (producer), get (consumer), and status (monitor) commands
- **Error Handling**: Proper response parsing and error reporting
- **Flexible Usage**: Command-line interface with parameters
- **Connection Management**: Automatic socket creation and cleanup
- **Stateless Operation**: Each client invocation is independent

## Queue Operations

### Send Operation
```bash
./queue_client 127.0.0.1 8003 send "Your message here"
```
- Adds message to the end of the queue
- Returns "OK" on success, "FULL" if queue is full

### Get Operation
```bash
./queue_client 127.0.0.1 8003 get
```
- Retrieves and removes message from front of queue
- Returns "msg:content" on success, "EMPTY" if queue is empty

### Status Operation
```bash
./queue_client 127.0.0.1 8003 status
```
- Returns current queue utilization as "count/max"
- Useful for monitoring queue depth

## Test Scripts

### `start_queue_server.sh`
- Starts the queue server on port 8003
- Handles PID management and process cleanup
- Provides startup status and available commands

### `test_queue_client.sh`
- Tests complete queue operations sequence
- Shows send, get, and status operations  
- Includes queue full and empty scenarios

## Network Configuration

### Default Settings
- **Server IP**: 127.0.0.1 (localhost)
- **Server Port**: 8003
- **Queue Size**: 10 messages maximum
- **Message Size**: 256 characters maximum

### Customization
```bash
# Use different server IP/port
./queue_client 192.168.1.100 8003 send "message"

# Start server on different port
./queue_server 8004
```

## Queue Characteristics

### Performance
- **Low Latency**: Direct UDP communication
- **High Throughput**: No connection overhead
- **Scalability**: Multiple producers and consumers

### Reliability
- **Message Persistence**: Messages stored until consumed
- **FIFO Ordering**: Guaranteed first-in-first-out delivery
- **Error Detection**: Proper error responses for all conditions
- **Queue Management**: Overflow protection and status monitoring

### Limitations
- **No Durability**: Messages lost if server crashes
- **No Acknowledgments**: UDP best-effort delivery
- **Single Queue**: One global queue for all clients
- **No Prioritization**: All messages have equal priority


## Related Documentation

This implementation demonstrates key concepts in UDP-based message queue systems:
- Message Queue Pattern architecture and design principles
- Performance considerations and reliability analysis
- Producer-consumer communication patterns over UDP
