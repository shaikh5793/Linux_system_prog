# Part 2: poll() - Modern I/O Multiplexing

 

## Overview

This part introduces `poll()`, a more scalable and intuitive alternative to `select()` for I/O multiplexing.

## Learning Objectives

After completing this part, you should understand:
- How `poll()` improves upon `select()` limitations
- The `struct pollfd` interface and event flags
- Timeout mechanisms in `poll()`
- When to choose `poll()` over `select()`
- The scalability advantages of `poll()`

## Examples

### 1. poll1.c - poll() with Timeout
**Concept:** Basic poll() usage with timeout

Demonstrates `poll()` fundamentals:
- Uses `struct pollfd` array instead of fd_set
- Monitors multiple file descriptors efficiently
- Implements 10-second timeout mechanism
- Shows event detection and reporting

**Run:**
```bash
# Test with data arrival
./poll1 &
echo "poll test" > pone

# Test timeout behavior  
./poll1
# Wait 10 seconds to see timeout
```

### 2. poll2.c - poll() Infinite Timeout
**Concept:** Continuous monitoring with poll()

Shows `poll()` in continuous monitoring mode:
- Uses infinite timeout (-1)
- Demonstrates event loop pattern
- Handles multiple events efficiently
- Shows how to break out of polling loop

**Run:**
```bash
./poll2 &
echo "first message" > pone
echo "second message" > ptwo
kill %1  # Stop the infinite loop
```

## Key Concepts Learned

### poll() System Call
```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

- **fds**: Array of file descriptor structures
- **nfds**: Number of descriptors in array
- **timeout**: Milliseconds to wait (-1 = infinite, 0 = no wait)

### struct pollfd
```c
struct pollfd {
    int fd;        /* file descriptor */
    short events;  /* requested events */
    short revents; /* returned events */
};
```

### Event Flags
- **POLLIN**: Data available for reading
- **POLLOUT**: Ready for writing
- **POLLERR**: Error condition
- **POLLHUP**: Hang up occurred
- **POLLNVAL**: Invalid request

### Advantages over select()
- No FD_SETSIZE limitation
- More intuitive interface  
- Better event information (revents)
- No need to rebuild fd sets
- More portable across Unix systems

### Limitations
- Still O(n) scanning in kernel
- Not as efficient as epoll for large numbers of FDs
- Timeout in milliseconds only

## Setup

### Prerequisites
```bash
# Create named pipes for testing
mkfifo pone ptwo
```

### Build
```bash
make all
```

### Test
```bash
make test
```

### Clean
```bash
make clean
```

## Exercises

### Beginner
1. Add a third file descriptor to `poll1.c`
2. Experiment with different timeout values
3. Handle POLLHUP and POLLERR events properly

### Intermediate
1. Create a simple server using `poll()`
2. Implement file watching with `poll()`
3. Compare performance with Part 1's `select()` examples

### Advanced
1. Build a multi-client chat server using `poll()`
2. Implement a web server with `poll()`
3. Handle both network and file I/O simultaneously

## Performance Comparison

| Feature | select() | poll() |
|---------|----------|--------|
| FD Limit | FD_SETSIZE (1024) | No limit |
| Interface | fd_set macros | struct pollfd |
| Rebuilding | Required each call | Not required |
| Event Info | Limited | Detailed (revents) |
| Portability | Good | Better |
| Performance | O(n) | O(n) |

## Next Steps

Once you understand `poll()`:
- **Part 3:** Learn about `epoll` - Linux's high-performance solution
- **Part 4:** Explore advanced Linux file descriptors
- Compare all three approaches for your use case

## Reference

- `man 2 poll` - poll() system call
- `man 7 poll` - poll() overview
- "Advanced Programming in the UNIX Environment" - Stevens & Rago
