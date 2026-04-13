# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O2
LDFLAGS :=

# Source files for Part 2
SOURCES := poll1.c poll2.c
BINARIES := $(SOURCES:.c=)

# Default target
all: setup $(BINARIES)
	@echo "Part 2 examples built successfully!"
	@echo "Run 'make test' to verify functionality"

# Build each program
$(BINARIES): %: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Setup named pipes
setup:
	@echo "Setting up named pipes for Part 2..."
	@mkfifo pone ptwo 2>/dev/null || true
	@chmod 666 pone ptwo 2>/dev/null || true

# Test target
test: all
	@echo "Testing Part 2 examples..."
	@echo "1. Testing poll() with timeout (poll1):"
	@timeout 3s ./poll1 &
	@sleep 1
	@echo "poll test data" > pone || true
	@wait 2>/dev/null || true
	@echo ""
	@echo "2. Testing poll() infinite timeout (poll2):"
	@timeout 5s ./poll2 &
	@sleep 1
	@echo "message 1" > pone || true
	@sleep 1
	@echo "message 2" > ptwo || true
	@sleep 1
	@kill %% 2>/dev/null || true
	@echo ""
	@echo "Part 2 tests completed!"

# Clean target
clean:
	rm -f $(BINARIES)
	rm -f pone ptwo

# Help target
help:
	@echo "Part 2: poll() - Modern I/O Multiplexing"
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build all Part 2 examples (default)"
	@echo "  test    - Run basic functionality tests"
	@echo "  clean   - Remove built binaries and named pipes"
	@echo "  help    - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  poll1   - poll() with timeout demonstration"
	@echo "  poll2   - poll() with infinite timeout"
	@echo ""
	@echo "Key improvements over select():"
	@echo "  - No FD_SETSIZE limit"
	@echo "  - Better event interface (struct pollfd)"
	@echo "  - More detailed event information"
	@echo ""
	@echo "Usage:"
	@echo "  make all && ./poll1"
	@echo "  echo 'test' > pone  # (in another terminal)"

.PHONY: all test clean help setup
