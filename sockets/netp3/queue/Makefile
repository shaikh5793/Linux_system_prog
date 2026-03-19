# Makefile for UDP Message Queue System
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Source files
SOURCES = queue_server.c queue_client.c

# Executables
TARGETS = queue_server queue_client

# Default target
all: $(TARGETS)

# Individual targets
queue_server: queue_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

queue_client: queue_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Convenience targets
server: queue_server

client: queue_client

# Start server using script
start: queue_server
	@echo "Starting queue server using startup script..."
	./start_queue_server.sh

# Run client test script
test-client: queue_client
	@echo "Running queue client test..."
	./test_queue_client.sh

# Run automated tests
test: $(TARGETS)
	@echo "Running queue system tests..."
	@echo "Use test scripts: ./start_queue_server.sh and ./test_queue_client.sh"



# Show available test scripts
scripts:
	@echo "=== Available Test Scripts ==="
	@echo "start_queue_server.sh   - Start server with PID management"
	@echo "test_queue_client.sh    - Complete queue operations test"
	@echo ""
	@echo "Usage examples:"
	@echo "  ./start_queue_server.sh        # Terminal 1"
	@echo "  ./test_queue_client.sh         # Terminal 2"

# Show queue operations
operations:
	@echo "=== Queue Operations ==="
	@echo ""
	@echo "Send Operations:"
	@echo "  ./queue_client 127.0.0.1 8003 send \"message\""
	@echo "  Response: OK (success) or FULL (queue full)"
	@echo ""
	@echo "Get Operations:"
	@echo "  ./queue_client 127.0.0.1 8003 get"
	@echo "  Response: msg:content (success) or EMPTY (no messages)"
	@echo ""
	@echo "Status Operations:"
	@echo "  ./queue_client 127.0.0.1 8003 status"
	@echo "  Response: count/max (e.g., \"3/10\")"
	@echo ""
	@echo "Queue Characteristics:"
	@echo "  - FIFO (First In, First Out) ordering"
	@echo "  - Maximum 10 messages"
	@echo "  - In-memory storage"
	@echo "  - Message persistence until consumed"

# Help
help:
	@echo "UDP Message Queue System"
	@echo "======================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all         - Build both server and client (default)"
	@echo "  server      - Build queue server only"
	@echo "  client      - Build queue client only"
	@echo ""
	@echo "Run Targets:"
	@echo "  start       - Start server using startup script"
	@echo "  test-client - Run client test script"
	@echo "  test        - Show testing instructions"
	@echo ""
	@echo ""
	@echo "Information:"
	@echo "  scripts     - List available test scripts"
	@echo "  operations  - Show queue operations and examples"
	@echo "  help        - Show this help"
	@echo "  clean       - Remove executables"
	@echo ""
	@echo "Quick Start:"
	@echo "  make all && make start     # Terminal 1"
	@echo "  make test-client           # Terminal 2"
	@echo "  make interactive           # Terminal 3 (optional)"

# Clean
clean:
	rm -f $(TARGETS)
	@echo "Cleaned queue system executables"

.PHONY: all server client start test-client test scripts operations help clean