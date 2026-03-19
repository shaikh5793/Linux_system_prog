# Makefile for UDP Communication Patterns
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Source files in main directory (basic examples)
BASIC_SOURCES = basic_udp_server.c basic_udp_client.c
BROADCAST_SOURCES = broadcast_server.c broadcast_client.c

# Executables in main directory
BASIC_TARGETS = basic_udp_server basic_udp_client
BROADCAST_TARGETS = broadcast_server broadcast_client

MAIN_TARGETS = $(BASIC_TARGETS) $(BROADCAST_TARGETS)

# Default target - build main directory examples only
all: $(MAIN_TARGETS)

# Individual main directory targets
basic_udp_server: basic_udp_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

basic_udp_client: basic_udp_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

broadcast_server: broadcast_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

broadcast_client: broadcast_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


# Core examples (basic UDP patterns)
core: $(BASIC_TARGETS) $(BROADCAST_TARGETS)

# Test targets
test-basic: basic_udp_server basic_udp_client
	@echo "=== Testing Basic UDP Communication ==="
	@echo "Start server in one terminal: ./basic_udp_server"
	@echo "Run client in another: ./basic_udp_client \"Hello UDP\""

test-broadcast: broadcast_server broadcast_client
	@echo "=== Testing UDP Broadcasting ==="
	@echo "Start server in one terminal: ./broadcast_server 8001"
	@echo "Run client in another: ./broadcast_client 8001"

test-discovery:
	@echo "=== Testing Service Discovery ==="
	@echo "cd discovery && make all && make start    # Terminal 1"
	@echo "cd discovery && make test-client          # Terminal 2"
	@echo "cd discovery && make test                 # Automated tests"

test-event:
	@echo "=== Testing Event Notification ==="
	@echo "cd event && make all && make start    # Terminal 1"
	@echo "cd event && make test-client          # Terminal 2"
	@echo "cd event && make test                 # Automated tests"

test-queue:
	@echo "=== Testing Message Queue ==="
	@echo "cd queue && make all && make start    # Terminal 1"
	@echo "cd queue && make test-client          # Terminal 2"
	@echo "cd queue && make test                 # Automated tests"

test-connected:
	@echo "=== Testing Connected UDP ==="
	@echo "cd connected && make all              # Build first"
	@echo "cd connected && ./connected_server 8006    # Terminal 1"
	@echo "cd connected && ./connected_client 127.0.0.1 8006    # Terminal 2"

# Test overview
test-overview:
	@echo "=== UDP Communication Patterns ==="
	@echo ""
	@echo "1. Basic UDP (basic_udp_*):"
	@echo "   - Connectionless communication"
	@echo "   - Message boundaries preserved"
	@echo "   - Use: make test-basic"
	@echo ""
	@echo "2. UDP Broadcasting (broadcast_*):"
	@echo "   - One-to-many communication"
	@echo "   - Network-wide message distribution"
	@echo "   - Use: make test-broadcast"
	@echo ""
	@echo "3. Event Notification System (event/):"
	@echo "   - Publish-subscribe pattern"
	@echo "   - Client registration and event distribution"
	@echo "   - Use: make test-event"
	@echo ""
	@echo "4. Message Queue System (queue/):"
	@echo "   - Producer-consumer pattern"
	@echo "   - FIFO message handling with persistence"
	@echo "   - Use: make test-queue"
	@echo ""
	@echo "5. Service Discovery (discovery/):"
	@echo "   - Automatic service location"
	@echo "   - Network service announcements"
	@echo "   - Use: make test-discovery"
	@echo ""
	@echo "6. Connected UDP (connected/):"
	@echo "   - Simplified UDP API using connect()"
	@echo "   - Stream-like interface over UDP"
	@echo "   - Use: make test-connected"
	@echo ""
	@echo "Quick Start:"
	@echo "  make all      # Build everything"
	@echo "  make help     # Show detailed help"

# Performance information
benchmark:
	@echo "=== UDP Performance Characteristics ==="
	@echo ""
	@echo "Pattern Comparison:"
	@echo "┌──────────────┬────────────┬────────────┬────────────┬─────────────────┐"
	@echo "│   Pattern    │  Topology  │ Delivery   │   State    │    Use Cases    │"
	@echo "├──────────────┼────────────┼────────────┼────────────┼─────────────────┤"
	@echo "│ Basic UDP    │ 1-to-1     │ Unicast    │ Stateless  │ Simple req/resp │"
	@echo "│ Broadcast    │ 1-to-many  │ Broadcast  │ Stateless  │ Announcements   │"
	@echo "│ Event System │ 1-to-many  │ Selective  │ Stateful   │ Pub/Sub systems │"
	@echo "│ Message Queue│ many-to-many│ Queued    │ Stateful   │ Async messaging │"
	@echo "│ Discovery    │ many-to-many│ Broadcast+ │ Stateful   │ Service location│"
	@echo "│ Connected    │ 1-to-1     │ Unicast    │ Stateless  │ Simplified API  │"
	@echo "└──────────────┴────────────┴────────────┴────────────┴─────────────────┘"
	@echo ""
	@echo "UDP vs TCP Benefits:"
	@echo "- Lower latency (no connection establishment)"
	@echo "- Reduced overhead (no connection state)"
	@echo "- Message boundaries preserved"
	@echo "- Broadcasting support for one-to-many patterns"
	@echo "- Ideal for event systems and real-time notifications"

# Clean up
clean:
	rm -f $(MAIN_TARGETS)
	@echo "Cleaned main directory executables"

# Help
help:
	@echo "UDP Communication Patterns Build System"
	@echo "Author: Raghu Bharadwaj (raghu@techveda.org)"
	@echo ""
	@echo "Build Targets:"
	@echo "  all        - Build main directory examples (default)"
	@echo "  core       - Build core examples (basic + broadcast)"
	@echo ""
	@echo ""
	@echo "Main Directory Examples:"
	@echo "  basic_udp_server / basic_udp_client       - Basic UDP communication"
	@echo "  broadcast_server / broadcast_client - UDP broadcasting"
	@echo ""
	@echo "Subdirectory Examples (each has independent Makefile):"
	@echo "  event/     - Event notification system (cd event && make)"
	@echo "  queue/     - Message queue system (cd queue && make)"
	@echo "  connected/ - Connected UDP examples (cd connected && make)"
	@echo "  discovery/ - Service discovery system (cd discovery && make)"
	@echo ""
	@echo "Testing Targets:"
	@echo "  test-basic      - Instructions for basic UDP testing"
	@echo "  test-broadcast  - Instructions for broadcast testing"
	@echo "  test-event      - Instructions for event system testing"
	@echo "  test-queue      - Instructions for message queue testing"
	@echo "  test-discovery  - Instructions for service discovery testing"
	@echo "  test-connected  - Instructions for connected UDP testing"
	@echo ""
	@echo "Other Targets:"
	@echo "  test-overview - Show pattern overview and usage examples"
	@echo "  benchmark  - Show UDP performance characteristics"
	@echo "  clean      - Remove main directory executables"
	@echo "  help       - Show this help"
	@echo ""
	@echo "Quick Start:"
	@echo "  make all && make test-overview"

# Quick test overview
test: core
	@echo "=== UDP Pattern Testing Overview ==="
	@echo ""
	@echo "Main Directory Examples:"
	@echo "  make test-basic      # Basic UDP communication"
	@echo "  make test-broadcast  # UDP broadcasting"
	@echo ""
	@echo "Subdirectory Examples (build independently):"
	@echo "  make test-event      # Event notification (cd event && make)"
	@echo "  make test-queue      # Message queue (cd queue && make)" 
	@echo "  make test-discovery  # Service discovery (cd discovery && make)"
	@echo "  make test-connected  # Connected UDP (cd connected && make)"
	@echo ""
	@echo "Complete Overview:"
	@echo "  make test-overview   # Show all patterns and usage"

.PHONY: all core test test-basic test-broadcast test-discovery test-event test-queue test-connected test-overview benchmark clean help