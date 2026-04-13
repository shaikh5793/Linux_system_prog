# Part 1: Basic I/O and select()


## Overview

This part introduces the fundamental problem of handling multiple I/O sources and demonstrates the traditional `select()` solution.

## Learning Objectives

After completing this part, you should understand:
- The limitations of blocking I/O when handling multiple sources
- How `select()` enables I/O multiplexing
- The difference between `select()` and `pselect()`
- File descriptor sets and timeout mechanisms
- The scalability limitations of `select()`

## Examples

### 1. multi1.c - The Problem
**Concept:** Blocking I/O limitation

This example demonstrates why blocking I/O doesn't work well with multiple sources:
- Opens two named pipes (`pone` and `ptwo`)
- Blocks on the first read, cannot handle concurrent input
- Shows sequential processing bottleneck

**Run:**
```bash
# Terminal 1
./multi1

# Terminal 2
echo "hello" > pone
echo "world" > ptwo
```

### 2. multi2.c - select() Solution
**Concept:** I/O multiplexing with timeout

Introduces `select()` to monitor multiple file descriptors:
- Uses `fd_set` to specify which descriptors to monitor
- Implements timeout mechanism (5 seconds)
- Non-blocking operation with event-driven processing

**Run:**
```bash
# Test with data
./multi2 &
echo "test data" > pone

# Test timeout behavior
./multi2
# Wait 5 seconds to see timeout message
```

### 3. multi3.c - pselect() with Signals
**Concept:** Signal-safe I/O multiplexing

Demonstrates `pselect()` for atomic signal handling:
- Blocks `SIGINT` and `SIGQUIT` during normal operation
- Atomically unblocks signals during `pselect()`
- Handles interrupted system calls gracefully

**Run:**
```bash
./multi3 &
echo "signal test" > pone
# Try Ctrl+C to see signal handling
```

## Key Concepts Learned

### select() System Call
```c
int select(int nfds, fd_set *readfds, fd_set *writefds, 
           fd_set *exceptfds, struct timeval *timeout);
```

- **nfds**: Highest file descriptor + 1
- **readfds**: Set of descriptors to check for read readiness
- **timeout**: Maximum time to wait (NULL = indefinite)

### File Descriptor Sets
- `FD_ZERO()` - Clear the set
- `FD_SET()` - Add descriptor to set  
- `FD_ISSET()` - Test if descriptor is in set
- `FD_CLR()` - Remove descriptor from set

### Limitations
- `FD_SETSIZE` limit (typically 1024 descriptors)
- O(n) scanning of file descriptors
- FD set copying overhead on each call
- Not portable across all Unix systems

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

### Clean
```bash
make clean
```

## Exercises

### Beginner
1. Modify `multi2.c` to monitor a third named pipe
2. Change timeout values and observe behavior
3. Add proper error recovery for failed `select()` calls

### Intermediate  
1. Create a simple chat server using `select()`
2. Implement a file monitoring system
3. Compare performance with different numbers of file descriptors

### Advanced
1. Implement a web server using `select()`
2. Handle both network sockets and local files
3. Add signal handling using `pselect()`

## Reference

- `man 2 select` - select() system call
- `man 2 pselect` - pselect() system call  
- `man 3 FD_SET` - File descriptor set macros
