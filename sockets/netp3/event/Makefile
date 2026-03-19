# Makefile for UDP Event Notification System
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Source files
SOURCES = event_server.c event_client.c

# Executables
TARGETS = event_server event_client

# Default target
all: $(TARGETS)

# Individual targets
event_server: event_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

event_client: event_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Convenience targets
server: event_server

client: event_client

# Start server using script
start: event_server
	@echo "Starting event server using startup script..."
	./start_event_server.sh

# Run client test script
test-client: event_client
	@echo "Running event client test..."
	./test_event_client.sh

# Run multiple clients test
multi-test: event_client
	@echo "Running multiple clients test..."
	./event_multi_client.sh

# Run automated tests
test: $(TARGETS)
	@echo "Running event system tests..."
	./event_test.sh


# Show available test scripts
scripts:
	@echo "=== Available Test Scripts ==="
	@echo "start_event_server.sh   - Start server with PID management"
	@echo "test_event_client.sh    - Single client test (20 seconds)"
	@echo "event_multi_client.sh   - Multiple clients test (3 clients)"
	@echo "event_test.sh           - Automated test suite"
	@echo ""
	@echo "Usage examples:"
	@echo "  ./start_event_server.sh        # Terminal 1"
	@echo "  ./test_event_client.sh         # Terminal 2"
	@echo "  ./event_multi_client.sh        # Terminal 3 (optional)"
	@echo "  ./event_test.sh                # Automated tests"

# Help
help:
	@echo "UDP Event Notification System"
	@echo "============================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all         - Build both server and client (default)"
	@echo "  server      - Build event server only"
	@echo "  client      - Build event client only"
	@echo ""
	@echo "Run Targets:"
	@echo "  start       - Start server using startup script"
	@echo "  test-client - Run client test script"
	@echo "  multi-test  - Run multiple clients test"
	@echo "  test        - Run automated test suite"
	@echo ""
	@echo "Information:"
	@echo "  scripts     - List available test scripts"
	@echo "  help        - Show this help"
	@echo "  clean       - Remove executables"
	@echo ""
	@echo "Quick Start:"
	@echo "  make all && make start     # Terminal 1"
	@echo "  make test-client           # Terminal 2"

# Clean
clean:
	rm -f $(TARGETS)
	@echo "Cleaned event system executables"

.PHONY: all server client start test-client multi-test test scripts help clean