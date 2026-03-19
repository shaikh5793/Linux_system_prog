# Makefile for Connected UDP Implementation
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Source files
SOURCES = connected_server.c connected_client.c

# Executables
TARGETS = connected_server connected_client

# Default target
all: $(TARGETS)

# Individual targets
connected_server: connected_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

connected_client: connected_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Convenience targets
server: connected_server

client: connected_client

# Start server using script
start: connected_server
	@echo "Starting connected UDP server using startup script..."
	./start_connected_server.sh

# Run test client
test-client: connected_client
	@echo "Running connected UDP client test..."
	./test_connected_client.sh

# Run automated tests
test: $(TARGETS)
	@echo "Running connected UDP tests..."
	./connected_test.sh


# Demonstrate connected UDP benefits
demo-benefits: $(TARGETS)
	@echo "=== Connected UDP Benefits Demonstration ==="
	@echo ""
	@echo "This example demonstrates:"
	@echo ""
	@echo "1. Simplified API:"
	@echo "   - Client uses send()/recv() instead of sendto()/recvfrom()"
	@echo "   - No need to specify destination address repeatedly"
	@echo ""
	@echo "2. Enhanced Error Reporting:"
	@echo "   - ICMP errors delivered to connected socket"
	@echo "   - Better error detection and handling"
	@echo ""
	@echo "3. Security Benefits:"
	@echo "   - Only receives packets from connected address"
	@echo "   - Prevents receiving packets from other sources"
	@echo ""
	@echo "4. Performance Optimization:"
	@echo "   - Kernel caches destination address"
	@echo "   - Reduced address lookup overhead"
	@echo ""
	@echo "Start the test to see these benefits in action:"
	@echo "  make quick-test"

# Show available test scripts
scripts:
	@echo "=== Available Test Scripts ==="
	@echo "start_connected_server.sh   - Start server with PID management"
	@echo "test_connected_client.sh    - Interactive client test"
	@echo "connected_test.sh           - Automated test suite"
	@echo ""
	@echo "Usage examples:"
	@echo "  ./start_connected_server.sh        # Terminal 1"
	@echo "  ./test_connected_client.sh         # Terminal 2"
	@echo "  ./connected_test.sh                # Automated tests"

# Show API comparison
api-comparison:
	@echo "=== UDP API Comparison ==="
	@echo ""
	@echo "Regular UDP Client:"
	@echo "  socket(AF_INET, SOCK_DGRAM, 0);"
	@echo "  sendto(sockfd, data, len, 0, &server_addr, addr_len);"
	@echo "  recvfrom(sockfd, buffer, len, 0, &from_addr, &addr_len);"
	@echo ""
	@echo "Connected UDP Client:"
	@echo "  socket(AF_INET, SOCK_DGRAM, 0);"
	@echo "  connect(sockfd, &server_addr, addr_len);  // \"Connect\" to server"
	@echo "  send(sockfd, data, len, 0);               // Simplified send"
	@echo "  recv(sockfd, buffer, len, 0);             // Simplified receive"
	@echo ""
	@echo "Server (unchanged for both):"
	@echo "  socket(AF_INET, SOCK_DGRAM, 0);"
	@echo "  bind(sockfd, &server_addr, addr_len);"
	@echo "  recvfrom(sockfd, buffer, len, 0, &client_addr, &addr_len);"
	@echo "  sendto(sockfd, response, len, 0, &client_addr, addr_len);"

# Show connected UDP limitations
limitations:
	@echo "=== Connected UDP Limitations ==="
	@echo ""
	@echo "Important to understand:"
	@echo ""
	@echo "1. Still Unreliable:"
	@echo "   - UDP semantics unchanged (packet loss, reordering)"
	@echo "   - No delivery guarantees"
	@echo "   - No duplicate protection"
	@echo ""
	@echo "2. No Actual Connection:"
	@echo "   - No connection state maintained"
	@echo "   - No handshake protocol"
	@echo "   - No flow control"
	@echo ""
	@echo "3. Scope Limitations:"
	@echo "   - Can only communicate with one peer at a time"
	@echo "   - Cannot send broadcasts when connected"
	@echo "   - Must disconnect/reconnect for new destination"
	@echo ""
	@echo "4. One-to-One Communication:"
	@echo "   - Cannot receive from multiple sources"
	@echo "   - Different from regular UDP's flexibility"

# Help
help:
	@echo "Connected UDP Implementation"
	@echo "==========================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all         - Build both server and client (default)"
	@echo "  server      - Build connected UDP server only"
	@echo "  client      - Build connected UDP client only"
	@echo ""
	@echo "Run Targets:"
	@echo "  start        - Start server using startup script"
	@echo "  test-client  - Run client test script"
	@echo "  test         - Run automated test suite"
	@echo ""
	@echo "Educational Targets:"
	@echo "  demo-benefits   - Explain connected UDP benefits"
	@echo "  api-comparison  - Show API differences"
	@echo "  limitations     - Explain connected UDP limitations"
	@echo ""
	@echo "Test Scripts:"
	@echo "  scripts     - List available test scripts"
	@echo ""
	@echo "Information:"
	@echo "  help        - Show this help"
	@echo "  clean       - Remove executables"
	@echo ""
	@echo "Quick Start:"
	@echo "  make all && make start         # Terminal 1"
	@echo "  make test-client               # Terminal 2"
	@echo ""
	@echo "Educational Overview:"
	@echo "  make demo-benefits    # Learn about benefits"
	@echo "  make api-comparison   # See API differences"
	@echo "  make limitations      # Understand limitations"

# Clean
clean:
	rm -f $(TARGETS)
	@echo "Cleaned connected UDP executables"

.PHONY: all server client start test-client test scripts demo-benefits api-comparison limitations help clean