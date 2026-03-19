# Linux Multi-I/O Examples

**Copyright (c) 2024 Techveda, Hyderabad, India**  
**Author: Raghu Bharadwaj | www.techveda.org**

## Course Structure

This repository contains a comprehensive set of Linux I/O multiplexing examples organized into progressive learning modules:

### [Part 1: Basic I/O and select()](part1/)
- Blocking I/O fundamentals
- `select()` system call
- `pselect()` with signal handling
- **Files**: `multi1.c`, `multi2.c`, `multi3.c`

### [Part 2: poll() - Modern I/O Multiplexing](part2/)
- `poll()` advantages over `select()`
- Better event interface
- No file descriptor limits
- **Files**: `poll1.c`, `poll2.c`

### [Part 3: epoll() - High-Performance I/O Multiplexing](part3/)
- Linux-specific `epoll()` API
- Level-triggered vs edge-triggered modes
- High-performance server design
- **Files**: `epoll.c`, `epoll_write.c`, `epoll_et_lt_helper.c`

### [Part 4: Advanced Linux File Descriptors](part4/)
- `signalfd` - Signal file descriptors
- `timerfd` - Timer file descriptors  
- `eventfd` - Event notification
- Modern Linux I/O interfaces
- **Files**: `signalfd_example.c`, `timerfd_example.c`, `eventfd_example.c`, `eventfd_self_test.c`, `test_eventfd.c`

## Quick Start

```bash
# Navigate to any part and build
cd part1
make all

# Run tests
make test

# Get help
make help
```

## Learning Path

1. **Start with [Part 1](part1/)** - Learn I/O fundamentals and `select()`
2. **Progress to [Part 2](part2/)** - Understand `poll()` improvements  
3. **Advance to [Part 3](part3/)** - Master high-performance `epoll()`
4. **Complete with [Part 4](part4/)** - Explore modern Linux file descriptors

Each part contains:
- Comprehensive README.md with theory and examples
- Working Makefile with build and test targets
- Progressive exercises and challenges
- Complete source code with detailed comments
