# Part 4: Advanced Linux File Descriptors - Makefile
# Copyright (c) 2024 Techveda, Hyderabad, India
# Author: Raghu Bharadwaj | www.techveda.org

# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O2
LDFLAGS :=

# Source files for Part 4
SOURCES := signalfd_example.c timerfd_example.c eventfd_example.c eventfd_self_test.c test_eventfd.c
BINARIES := $(SOURCES:.c=)

# Default target
all: $(BINARIES)
	@echo "Part 4 examples built successfully!"
	@echo "Run 'make test' to verify functionality"

# Build each program
$(BINARIES): %: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Test target
test: all test-signalfd test-timerfd test-eventfd
	@echo "All Part 4 tests completed!"

# Test signalfd functionality
test-signalfd: signalfd_example
	@echo "Testing signalfd example..."
	@echo "Starting signalfd_example in background..."
	@timeout 5s ./signalfd_example &
	@sleep 1
	@echo "Sending SIGUSR1 signal..."
	@kill -USR1 $$! 2>/dev/null || true
	@sleep 1
	@echo "Sending SIGTERM signal..."
	@kill -TERM $$! 2>/dev/null || true
	@wait 2>/dev/null || true
	@echo "signalfd test completed!"
	@echo ""

# Test timerfd functionality  
test-timerfd: timerfd_example
	@echo "Testing timerfd example..."
	@timeout 3s ./timerfd_example || true
	@echo "timerfd test completed!"
	@echo ""

# Test eventfd functionality
test-eventfd: eventfd_example eventfd_self_test test_eventfd
	@echo "Testing eventfd examples..."
	@echo "1. Running eventfd_self_test:"
	@./eventfd_self_test || true
	@echo ""
	@echo "2. Testing eventfd_example with timeout:"
	@timeout 2s ./eventfd_example &
	@sleep 1
	@echo "Sending event to eventfd..."
	@./test_eventfd 2>/dev/null || true
	@wait 2>/dev/null || true
	@echo "eventfd tests completed!"
	@echo ""

# Individual test targets
test-only-signalfd: test-signalfd
test-only-timerfd: test-timerfd  
test-only-eventfd: test-eventfd


# Clean target
clean:
	rm -f $(BINARIES)

# Help target
help:
	@echo "Part 4: Advanced Linux File Descriptors"
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build all Part 4 examples (default)"
	@echo "  test             - Run all functionality tests"
	@echo "  test-signalfd    - Test signalfd functionality"
	@echo "  test-timerfd     - Test timerfd functionality" 
	@echo "  test-eventfd     - Test eventfd functionality"
	@echo "  clean            - Remove built binaries"
	@echo "  help             - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  signalfd_example  - Signal file descriptor demo"
	@echo "  timerfd_example   - Timer file descriptor demo"
	@echo "  eventfd_example   - Event file descriptor demo"
	@echo "  eventfd_self_test - Self-contained eventfd test"
	@echo "  test_eventfd      - Helper for eventfd testing"
	@echo ""
	@echo "Key features:"
	@echo "  - signalfd: Convert signals to file descriptor events"
	@echo "  - timerfd:  Create timers as file descriptors"
	@echo "  - eventfd:  Lightweight event notification"
	@echo "  - Integration with epoll/select/poll"
	@echo ""
	@echo "Usage:"
	@echo "  make all && ./timerfd_example"
	@echo "  make test-eventfd"

.PHONY: all test test-signalfd test-timerfd test-eventfd test-only-signalfd test-only-timerfd test-only-eventfd clean help
