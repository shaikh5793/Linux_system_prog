# Part 4: Advanced Linux File Descriptors


## Overview

This section covers advanced Linux file descriptor types that provide specialized functionality beyond traditional files and sockets. These modern interfaces allow applications to integrate timers, signals, and event notifications directly into their I/O multiplexing loops.

## Key Concepts

### Modern Linux File Descriptors

Linux provides several specialized file descriptor types that can be used with standard I/O multiplexing mechanisms:

- **signalfd**: Convert signals into file descriptor events
- **timerfd**: Create timers that deliver events via file descriptors
- **eventfd**: Lightweight event notification mechanism
- **inotify**: File system event monitoring
- **pidfd**: Process lifecycle monitoring

### Integration with I/O Multiplexing

These specialized file descriptors can be seamlessly integrated with:
- `select()`, `poll()`, `epoll()` for event-driven programming
- Standard read/write operations
- Non-blocking I/O patterns
- Event loops and reactor patterns

## Examples in This Part

### 1. signalfd_example.c - Signal File Descriptors
Demonstrates using `signalfd()` to handle signals synchronously:
- Converting asynchronous signals to synchronous events
- Integration with `epoll()` for unified event handling
- Handling multiple signal types
- Signal masking and delivery

### 2. timerfd_example.c - Timer File Descriptors
Shows `timerfd_create()` for timer management:
- Creating periodic and one-shot timers
- Reading timer expiration events
- Combining timers with other I/O events
- High-resolution timer support

### 3. eventfd_example.c - Event File Descriptors
Covers `eventfd()` for lightweight event notification:
- Creating event objects
- Semaphore-style and counter-style modes
- Thread synchronization
- Integration with event loops

### 4. eventfd_self_test.c - Self-Contained Event Test
Complete standalone test for eventfd functionality:
- No external dependencies
- Built-in test scenarios
- Comprehensive error handling
- Educational examples

### 5. inotify_example.c - File System Monitoring
Demonstrates `inotify()` for file system events:
- Monitoring file and directory changes
- Handling various event types
- Recursive directory watching
- Integration with I/O multiplexing

## Building and Testing

### Prerequisites
- Linux system (these are Linux-specific features)
- GCC compiler
- Linux kernel 2.6.22+ (for most features)

### Build Commands
```bash
# Build all examples
make all

# Build specific example
make signalfd_example

# Run comprehensive tests
make test

# Run specific tests
make test-signalfd
make test-timerfd
make test-eventfd

# Clean up
make clean
```

### Manual Testing
```bash
# Test signalfd
./signalfd_example
# Press Ctrl+C, send SIGUSR1, etc.

# Test timerfd
./timerfd_example
# Watch periodic timer events

# Test eventfd
./eventfd_example
# Observe event notifications

# Test inotify
./inotify_example /tmp
# Create/modify files in /tmp
```

## Key Learning Points

### 1. signalfd System Calls
- `signalfd()` - Create/modify signal file descriptor
- `sigprocmask()` - Block signals for synchronous handling
- `read()` - Read signal information
- `struct signalfd_siginfo` - Signal event data

### 2. timerfd System Calls
- `timerfd_create()` - Create timer file descriptor
- `timerfd_settime()` - Arm/disarm timer
- `timerfd_gettime()` - Get current timer value
- `read()` - Read timer expiration count

### 3. eventfd System Calls
- `eventfd()` - Create event file descriptor
- `write()` - Signal event (increment counter)
- `read()` - Wait for event (read counter)
- `EFD_CLOEXEC`, `EFD_NONBLOCK`, `EFD_SEMAPHORE` flags

### 4. inotify System Calls
- `inotify_init()` - Create inotify instance
- `inotify_add_watch()` - Add file/directory to watch
- `inotify_rm_watch()` - Remove watch
- `read()` - Read file system events

## Common Use Cases

### 1. Signal Integration
```c
// Replace signal handlers with synchronous processing
sigset_t mask;
int sfd;

sigemptyset(&mask);
sigaddset(&mask, SIGINT);
sigprocmask(SIG_BLOCK, &mask, NULL);
sfd = signalfd(-1, &mask, SFD_CLOEXEC);

// Use sfd with epoll/select/poll
```

### 2. Timer Management
```c
// Create periodic timer
int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
struct itimerspec timer = {
    .it_interval = {.tv_sec = 1, .tv_nsec = 0},
    .it_value = {.tv_sec = 1, .tv_nsec = 0}
};
timerfd_settime(tfd, 0, &timer, NULL);

// Integrate with event loop
```

### 3. Event Notification
```c
// Create event for thread communication
int efd = eventfd(0, EFD_CLOEXEC);

// Producer thread
uint64_t value = 1;
write(efd, &value, sizeof(value));

// Consumer thread (or event loop)
read(efd, &value, sizeof(value));
```

## Performance Considerations

### 1. Unified Event Loop
- Single `epoll()` instance for all event types
- Reduced context switching
- Better cache locality
- Simplified error handling

### 2. Non-blocking Operations
- Use `O_NONBLOCK` flags where appropriate
- Handle `EAGAIN`/`EWOULDBLOCK` properly
- Avoid blocking the event loop

### 3. Memory Management
- Pre-allocate event structures
- Use object pools for frequent operations
- Minimize dynamic allocation in hot paths

## Advanced Patterns

### 1. Reactor Pattern
```c
// Single event loop handling multiple event types
struct epoll_event events[MAX_EVENTS];
int epfd = epoll_create1(EPOLL_CLOEXEC);

// Add signalfd, timerfd, eventfd to epoll
epoll_ctl(epfd, EPOLL_CTL_ADD, signalfd, &event);
epoll_ctl(epfd, EPOLL_CTL_ADD, timerfd, &event);
epoll_ctl(epfd, EPOLL_CTL_ADD, eventfd, &event);

// Main event loop
while (running) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
        handle_event(&events[i]);
    }
}
```

### 2. State Machine Integration
- Use timers for state transitions
- Signal handling for external events
- Event notifications for internal state changes

## Exercises

### Exercise 1: Unified Event Server
Create a server that handles:
- Network connections (sockets)
- Periodic tasks (timerfd)
- Signal handling (signalfd)
- Inter-thread communication (eventfd)

### Exercise 2: File System Monitor
Build a file monitoring tool using inotify that:
- Watches multiple directories recursively
- Logs all file system events
- Handles watch removal/addition dynamically
- Integrates with a GUI or web interface

### Exercise 3: Process Manager
Implement a process manager using:
- signalfd for signal handling
- timerfd for periodic health checks
- eventfd for process communication
- inotify for configuration file changes

### Exercise 4: Real-time Logger
Create a high-performance logger that:
- Uses eventfd for log event notifications
- timerfd for periodic log rotation
- signalfd for graceful shutdown
- inotify for configuration reloading

## Common Pitfalls

1. **Forgetting to block signals before using signalfd**
2. **Not handling timer overruns properly**
3. **Misunderstanding eventfd semaphore mode**
4. **Not setting O_NONBLOCK for non-blocking operations**
5. **Ignoring error conditions in read/write operations**

## Further Reading

- Linux man pages: `man 2 signalfd`, `man 2 timerfd_create`, `man 2 eventfd`, `man 7 inotify`
- "The Linux Programming Interface" by Michael Kerrisk
- Linux kernel documentation
- "Advanced Programming in the UNIX Environment" by W. Richard Stevens

## References

- [Linux signalfd man page](https://man7.org/linux/man-pages/man2/signalfd.2.html)
- [Linux timerfd man page](https://man7.org/linux/man-pages/man2/timerfd_create.2.html)
- [Linux eventfd man page](https://man7.org/linux/man-pages/man2/eventfd.2.html)
- [Linux inotify man page](https://man7.org/linux/man-pages/man7/inotify.7.html)

---

**Previous**: Return to [Part 3: epoll() - High-Performance I/O Multiplexing](../part3/README.md)

**Overview**: Back to [Main README](../README.md) for complete course structure
