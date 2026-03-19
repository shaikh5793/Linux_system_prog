# Part 3: epoll() - High-Performance I/O Multiplexing - Makefile
# Copyright (c) 2024 Techveda, Hyderabad, India
# Author: Raghu Bharadwaj | www.techveda.org

# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O2
LDFLAGS :=

# Source files for Part 3
SOURCES := epoll.c epoll_write.c epoll_et_lt_helper.c
BINARIES := $(SOURCES:.c=)

# Default target
all: setup $(BINARIES)
	@echo "Part 3 examples built successfully!"
	@echo "Run 'make test' to verify functionality"

# Build each program
$(BINARIES): %: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Setup named pipes
setup:
	@echo "Setting up named pipes for Part 3..."
	@mkfifo pone ptwo 2>/dev/null || true
	@chmod 666 pone ptwo 2>/dev/null || true

# Test target
test: all
	@echo "Testing Part 3 examples..."
	@echo "1. Testing basic epoll (level-triggered):"
	@timeout 3s ./epoll &
	@sleep 1
	@echo "epoll test data" > pone || true
	@sleep 1
	@echo "more epoll data" > ptwo || true
	@wait 2>/dev/null || true
	@echo ""
	@echo "2. Testing epoll write events:"
	@timeout 3s ./epoll_write &
	@sleep 1
	@wait 2>/dev/null || true
	@echo ""
	@echo "3. Testing edge-triggered vs level-triggered helper:"
	@timeout 3s ./epoll_et_lt_helper &
	@sleep 1
	@echo "helper test" > pone || true
	@wait 2>/dev/null || true
	@echo ""
	@echo "Part 3 tests completed!"

# Performance test target
perf: all
	@echo "Running performance comparison..."
	@echo "Testing with 1000 events..."
	@for i in {1..1000}; do echo "event $$i"; done > pone &
	@./epoll
	@echo "Performance test completed!"

# Clean target
clean:
	rm -f $(BINARIES)
	rm -f pone ptwo

# Help target
help:
	@echo "Part 3: epoll() - High-Performance I/O Multiplexing"
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build all Part 3 examples (default)"
	@echo "  test    - Run basic functionality tests"
	@echo "  perf    - Run performance tests"
	@echo "  clean   - Remove built binaries and named pipes"
	@echo "  help    - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  epoll                - Basic epoll (level-triggered)"
	@echo "  epoll_write          - Write event handling"
	@echo "  epoll_et_lt_helper   - Edge vs level-triggered comparison"
	@echo ""
	@echo "Key advantages of epoll:"
	@echo "  - O(1) performance scaling"
	@echo "  - No file descriptor limits"
	@echo "  - Kernel-managed event structures"
	@echo "  - Edge and level-triggered modes"
	@echo ""
	@echo "Usage:"
	@echo "  make all && ./epoll"
	@echo "  echo 'data' > pone  # (in another terminal)"

.PHONY: all test perf clean help setup
