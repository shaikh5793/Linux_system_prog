# Part 3: epoll() - High-Performance I/O Multiplexing

**Copyright (c) 2024 Techveda, Hyderabad, India**  
**Author: Raghu Bharadwaj | www.techveda.org**

## Overview

This section covers **epoll()**, Linux's high-performance I/O multiplexing mechanism. epoll is designed to handle thousands of file descriptors efficiently, making it ideal for high-performance server applications.

## Key Concepts

### What is epoll?

epoll is a Linux-specific I/O event notification facility that provides:

- **O(1) performance** for adding/removing/checking file descriptors
- **Edge-triggered and level-triggered** event notification modes
- **Efficient memory usage** through kernel-managed event structures
- **Scalability** to handle thousands of connections

### epoll vs select/poll

| Feature | select() | poll() | epoll() |
|---------|----------|---------|---------|
| FD Limit | 1024 (FD_SETSIZE) | No limit | No limit |
| Performance | O(n) | O(n) | O(1) |
| Memory | User space | User space | Kernel space |
| Portability | POSIX | POSIX | Linux only |
| Event Types | Read/Write/Error | POLLIN/POLLOUT/etc | EPOLLIN/EPOLLOUT/etc |

## Examples in This Part

### 1. epoll.c - Basic epoll Usage
Demonstrates basic epoll functionality with level-triggered mode:
- Creating an epoll instance with `epoll_create1()`
- Adding file descriptors with `epoll_ctl()`
- Waiting for events with `epoll_wait()`
- Handling multiple concurrent connections

### 2. epoll_et.c - Edge-Triggered Mode
Shows epoll in edge-triggered mode:
- `EPOLLET` flag for edge-triggered notifications
- Non-blocking I/O requirements
- Proper handling of EAGAIN/EWOULDBLOCK
- Performance benefits and considerations

### 3. epoll_write.c - Write Event Handling
Demonstrates handling write events:
- `EPOLLOUT` for write readiness
- Managing output buffers
- Avoiding busy-wait on write events
- One-shot events with `EPOLLONESHOT`

### 4. epoll_level_vs_edge.c - Mode Comparison
Direct comparison between level-triggered and edge-triggered modes:
- Same logic, different event modes
- Performance characteristics
- Event notification differences
- Best practices for each mode

## Building and Testing

### Prerequisites
- Linux system (epoll is Linux-specific)
- GCC compiler
- Named pipes (created automatically)

### Build Commands
```bash
# Build all examples
make all

# Build specific example
make epoll

# Run tests
make test

# Clean up
make clean
```

### Manual Testing
```bash
# Terminal 1: Run epoll example
./epoll

# Terminal 2: Send data to pipes
echo "Hello epoll" > pone
echo "More data" > ptwo

# Test edge-triggered mode
./epoll_et

# Test write events
./epoll_write
```

## Key Learning Points

### 1. epoll System Calls
- `epoll_create1()` - Create epoll instance
- `epoll_ctl()` - Add/modify/remove file descriptors
- `epoll_wait()` - Wait for events

### 2. Event Modes
- **Level-triggered (default)**: Event reported while condition exists
- **Edge-triggered (EPOLLET)**: Event reported only on state changes

### 3. Performance Considerations
- Use edge-triggered mode for maximum performance
- Always use non-blocking I/O with edge-triggered mode
- Handle EAGAIN/EWOULDBLOCK properly
- Consider using `EPOLLONESHOT` for thread safety

### 4. Event Types
- `EPOLLIN` - Data available for reading
- `EPOLLOUT` - File descriptor ready for writing
- `EPOLLERR` - Error condition
- `EPOLLHUP` - Hang up happened
- `EPOLLET` - Edge-triggered mode
- `EPOLLONESHOT` - One-shot event

## Common Pitfalls

1. **Forgetting non-blocking I/O with edge-triggered mode**
2. **Not handling EAGAIN properly**
3. **Busy-waiting on EPOLLOUT events**
4. **Not checking for EPOLLERR and EPOLLHUP**
5. **Assuming epoll works on all platforms**

## Exercises

### Exercise 1: Basic epoll Server
Modify `epoll.c` to create a simple echo server that:
- Accepts multiple client connections
- Echoes received data back to clients
- Handles client disconnections gracefully

### Exercise 2: Edge-Triggered Performance
Compare the performance of `epoll.c` vs `epoll_et.c`:
- Use a script to send large amounts of data
- Measure CPU usage and response times
- Document the differences

### Exercise 3: Write Buffer Management
Extend `epoll_write.c` to implement:
- A proper output buffer system
- Handling of partial writes
- Efficient memory management

### Exercise 4: Real-world Application
Create a simple HTTP server using epoll that:
- Handles multiple concurrent connections
- Serves static files
- Uses edge-triggered mode for performance

## Performance Tips

1. **Use edge-triggered mode** for high-performance applications
2. **Batch epoll_wait() calls** to reduce system call overhead
3. **Use appropriate timeout values** based on your application needs
4. **Consider thread pools** for CPU-intensive processing
5. **Profile your application** to identify bottlenecks

## Further Reading

- Linux man pages: `man 7 epoll`, `man 2 epoll_create1`, `man 2 epoll_ctl`, `man 2 epoll_wait`
- "The Linux Programming Interface" by Michael Kerrisk
- Linux kernel documentation on epoll
- High-performance network programming guides

## References

- [Linux epoll man page](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [epoll vs select/poll performance comparison](https://www.kernel.org/doc/ols/2004/ols2004v1-pages-215-226.pdf)
- [C10K problem and solutions](http://www.kegel.com/c10k.html)

---

**Next**: Proceed to [Part 4: Advanced Linux File Descriptors](../part4/README.md) to learn about signalfd, timerfd, and eventfd.

**Previous**: Return to [Part 2: poll() - Modern I/O Multiplexing](../part2/README.md)
