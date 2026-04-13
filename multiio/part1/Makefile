# Part 1: Basic I/O and select() - Makefile
# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -O2
LDFLAGS :=

# Source files for Part 1
SOURCES := multi1.c multi2.c multi3.c
BINARIES := $(SOURCES:.c=)

# Default target
all: setup $(BINARIES)
	@echo "Part 1 examples built successfully!"
	@echo "Run 'make test' to verify functionality"

# Build each program
$(BINARIES): %: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Setup named pipes
setup:
	@echo "Setting up named pipes for Part 1..."
	@mkfifo pone ptwo 2>/dev/null || true
	@chmod 666 pone ptwo 2>/dev/null || true

# Test target
test: all
	@echo "Testing Part 1 examples..."
	@echo "1. Testing blocking I/O (multi1):"
	@echo "hello" > pone &
	@echo "world" > ptwo &
	@timeout 5s ./multi1 || echo "multi1 test completed"
	@echo ""
	@echo "2. Testing select() (multi2):"
	@timeout 3s ./multi2 &
	@sleep 1
	@echo "select test" > pone || true
	@wait 2>/dev/null || true
	@echo ""
	@echo "Part 1 tests completed!"

# Clean target
clean:
	rm -f $(BINARIES)
	rm -f pone ptwo

# Help target
help:
	@echo "Part 1: Basic I/O and select() Examples"
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build all Part 1 examples (default)"
	@echo "  test    - Run basic functionality tests"
	@echo "  clean   - Remove built binaries and named pipes"
	@echo "  help    - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  multi1  - Blocking I/O demonstration"
	@echo "  multi2  - select() with timeout"
	@echo "  multi3  - pselect() with signal handling"
	@echo ""
	@echo "Usage:"
	@echo "  make all && ./multi1"
	@echo "  echo 'test' > pone  # (in another terminal)"

.PHONY: all test clean help setup
