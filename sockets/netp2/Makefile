# Makefile for TCP Local IPC Examples
# Part 2: TCP Local IPC Examples
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Source files
BASIC_SOURCES = 01_basic_server.c 01_basic_client.c
MULTI_SOURCES = 02_multi_server.c 02_multi_client.c
THREADED_SOURCES = 03_threaded_server.c 03_threaded_client.c
HEARTBEAT_SOURCES = 04_heartbeat_server.c 04_heartbeat_client.c

# Executables
BASIC_TARGETS = 01_basic_server 01_basic_client
MULTI_TARGETS = 02_multi_server 02_multi_client
THREADED_TARGETS = 03_threaded_server 03_threaded_client
HEARTBEAT_TARGETS = 04_heartbeat_server 04_heartbeat_client

ALL_TARGETS = $(BASIC_TARGETS) $(MULTI_TARGETS) $(THREADED_TARGETS) $(HEARTBEAT_TARGETS)

# Default target
all: $(ALL_TARGETS)

# Basic TCP local IPC
01_basic_server: 01_basic_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

01_basic_client: 01_basic_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Multi-client TCP server
02_multi_server: 02_multi_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

02_multi_client: 02_multi_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Threaded TCP server (requires pthread)
03_threaded_server: 03_threaded_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lpthread

03_threaded_client: 03_threaded_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Heartbeat TCP server (requires pthread)
04_heartbeat_server: 04_heartbeat_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lpthread

04_heartbeat_client: 04_heartbeat_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Test targets
test-basic: 01_basic_server 01_basic_client
	@echo "=== Testing Basic TCP Local IPC ==="
	@echo "Terminal 1: ./01_basic_server 8080"
	@echo "Terminal 2: ./01_basic_client 127.0.0.1 8080 \"Hello TCP\""

test-multi: 02_multi_server 02_multi_client
	@echo "=== Testing Multi-Client TCP Server ==="
	@echo "Terminal 1: ./02_multi_server 8081"
	@echo "Terminal 2: ./02_multi_client 127.0.0.1 8081 \"Client-1\" &"
	@echo "Terminal 3: ./02_multi_client 127.0.0.1 8081 \"Client-2\" &"
	@echo "Terminal 4: ./02_multi_client 127.0.0.1 8081 \"Client-3\" &"

test-threaded: 03_threaded_server 03_threaded_client
	@echo "=== Testing Threaded TCP Server ==="
	@echo "Terminal 1: ./03_threaded_server 8082"
	@echo "Terminal 2: ./03_threaded_client 127.0.0.1 8082"
	@echo "(Interactive client - type messages, 'quit' to exit)"

test-heartbeat: 04_heartbeat_server 04_heartbeat_client
	@echo "=== Testing Heartbeat TCP Server ==="
	@echo "Terminal 1: ./04_heartbeat_server 8083"
	@echo "Terminal 2: ./04_heartbeat_client 127.0.0.1 8083"
	@echo "(Client sends heartbeat every 10 seconds, server monitors connections)"

# Demo with multiple clients
demo-concurrent: 02_multi_server 02_multi_client
	@echo "Starting concurrent client demo..."
	@echo "This will start the server and 3 clients automatically"
	./02_multi_server 8082 & \
	sleep 1; \
	./02_multi_client 127.0.0.1 8082 "Auto-Client-1" & \
	./02_multi_client 127.0.0.1 8082 "Auto-Client-2" & \
	./02_multi_client 127.0.0.1 8082 "Auto-Client-3" & \
	echo "Demo running... Press Ctrl+C to stop all processes"

# Performance test
perf-test: $(ALL_TARGETS)
	@echo "=== TCP vs Unix Domain Socket Performance ==="
	@echo "TCP Localhost characteristics:"
	@echo "- Uses network stack (slower than Unix sockets)"
	@echo "- Network protocol overhead"
	@echo "- More memory usage"
	@echo "- But: Network compatible, language agnostic"
	@echo ""
	@echo "Use cases for TCP local IPC:"
	@echo "- When same code needs to work locally and remotely"
	@echo "- Language interoperability (not all languages support Unix sockets)"
	@echo "- Existing TCP-based protocols/infrastructure"
	@echo "- Future distributed system planning"

# Port scanning utility
check-ports:
	@echo "=== Checking Common Ports ==="
	@netstat -tlnp 2>/dev/null | grep ':808[0-9]' || echo "Ports 8080-8089 appear to be free"
	@echo "Recommended test ports: 8080, 8081, 8082"

# Security note
security-info:
	@echo "=== TCP Local IPC Security Notes ==="
	@echo "1. Binding to 127.0.0.1 restricts to localhost only"
	@echo "2. Still accessible to all local processes"
	@echo "3. No built-in authentication (unlike Unix domain sockets)"
	@echo "4. Network-level firewall rules don't apply to localhost"
	@echo "5. Consider additional authentication for sensitive IPC"

# Clean up
clean:
	rm -f $(ALL_TARGETS)
	@echo "Cleaned executables"

# Kill any remaining processes
kill-servers:
	@echo "Killing any remaining server processes..."
	@pkill -f 01_basic_server || true
	@pkill -f 02_multi_server || true
	@echo "Server cleanup complete"

# Demo script
demo:
	@echo "=== TCP Local IPC Demo ==="
	@echo "This demonstrates various TCP socket features:"
	@echo ""
	@echo "1. Basic TCP (01_basic_*):"
	@echo "   - Simple client-server communication"
	@echo "   - Single client per connection"
	@echo "   - Use: ./01_basic_server 8080 & ./01_basic_client 127.0.0.1 8080 \"Hello\""
	@echo ""
	@echo "2. Multi-Client TCP (02_multi_*):"
	@echo "   - Fork-based concurrent server"
	@echo "   - Multiple simultaneous clients"
	@echo "   - Use: ./02_multi_server 8081 & ./02_multi_client 127.0.0.1 8081 \"Client-1\""
	@echo ""
	@echo "3. Threaded TCP (03_threaded_*):"
	@echo "   - Pthread-based concurrent server"
	@echo "   - Interactive client interface"
	@echo "   - Use: ./03_threaded_server 8082 & ./03_threaded_client 127.0.0.1 8082"
	@echo ""
	@echo "4. Heartbeat TCP (04_heartbeat_*):"
	@echo "   - Connection monitoring and keep-alive"
	@echo "   - Automatic client reconnection"
	@echo "   - Use: ./04_heartbeat_server 8083 & ./04_heartbeat_client 127.0.0.1 8083"

# Performance comparison
benchmark: $(ALL_TARGETS)
	@echo "=== TCP Performance Characteristics ==="
	@echo "TCP vs Unix Domain Sockets for local IPC:"
	@echo "- Network stack overhead (higher latency)"
	@echo "- Protocol reliability guarantees"
	@echo "- Network compatibility (can work remotely)"
	@echo "- Language interoperability"
	@echo "- Built-in flow control and congestion management"
	@echo "- Ideal for cross-language IPC and future distribution"

# Quick test overview
test: $(BASIC_TARGETS)
	@echo "=== TCP Socket Testing Overview ==="
	@echo "Use 'make test-<example>' for specific instructions, e.g.:"
	@echo "  make test-basic      # Basic TCP communication"
	@echo "  make test-multi      # Multi-client server"
	@echo "  make test-threaded   # Threaded server"
	@echo "  make test-heartbeat  # Heartbeat monitoring"
	@echo ""
	@echo "Or use 'make demo' for complete feature overview"

# Help
help:
	@echo "Socket Programming - Part 2: TCP Local IPC"
	@echo "Author: Raghu Bharadwaj (raghu@techveda.org)"
	@echo ""
	@echo "Build Targets:"
	@echo "  all        - Build all examples (default)"
	@echo ""
	@echo "Individual Examples:"
	@echo "  01_basic_server / 01_basic_client       - Basic TCP communication"
	@echo "  02_multi_server / 02_multi_client       - Multi-client with fork()"
	@echo "  03_threaded_server / 03_threaded_client - Multi-client with pthreads"
	@echo "  04_heartbeat_server / 04_heartbeat_client - Connection monitoring"
	@echo ""
	@echo "Testing Targets:"
	@echo "  test-basic      - Instructions for basic TCP testing"
	@echo "  test-multi      - Instructions for multi-client testing"
	@echo "  test-threaded   - Instructions for threaded server testing"
	@echo "  test-heartbeat  - Instructions for heartbeat testing"
	@echo ""
	@echo "Other:"
	@echo "  demo            - Show feature overview and usage examples"
	@echo "  benchmark       - Show TCP performance characteristics"
	@echo "  demo-concurrent - Automatic concurrent client demo"
	@echo "  check-ports     - Check port availability"
	@echo "  security-info   - Display security considerations"
	@echo "  kill-servers    - Kill any remaining server processes"
	@echo "  clean           - Remove executables"
	@echo "  help            - Show this help"

.PHONY: all clean test test-basic test-multi test-threaded test-heartbeat demo benchmark demo-concurrent perf-test check-ports security-info kill-servers help
