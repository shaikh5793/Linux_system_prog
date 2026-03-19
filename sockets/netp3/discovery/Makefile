# Makefile for UDP Discovery System
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Source files
SOURCES = discovery_server.c discovery_client.c

# Executables
TARGETS = discovery_server discovery_client

# Default target
all: $(TARGETS)

# Individual targets
discovery_server: discovery_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

discovery_client: discovery_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Convenience targets
server: discovery_server

client: discovery_client

# Start server using script
start: discovery_server
	@echo "Starting discovery server using startup script..."
	./start_discovery_server.sh

# Run client test script
test-client: discovery_client
	@echo "Running discovery client test..."
	./test_discovery_client.sh

# Run automated tests
test: $(TARGETS)
	@echo "Running discovery system tests..."
	./discovery_test.sh


# Show available test scripts
scripts:
	@echo "=== Available Test Scripts ==="
	@echo "start_discovery_server.sh   - Start server with PID management"
	@echo "test_discovery_client.sh    - Single client test"
	@echo "discovery_test.sh           - Automated test suite"
	@echo ""
	@echo "Usage examples:"
	@echo "  ./start_discovery_server.sh        # Terminal 1"
	@echo "  ./test_discovery_client.sh         # Terminal 2"
	@echo "  ./discovery_test.sh                # Automated tests"

# Help
help:
	@echo "UDP Discovery System"
	@echo "==================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all         - Build both server and client (default)"
	@echo "  server      - Build discovery server only"
	@echo "  client      - Build discovery client only"
	@echo ""
	@echo "Run Targets:"
	@echo "  start       - Start server using startup script"
	@echo "  test-client - Run client test script"
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
	@echo "Cleaned discovery system executables"

.PHONY: all server client start test-client test scripts help clean