# Makefile for Unix Domain Socket Examples
# Part 1: AF_UNIX Socket IPC Examples
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Source files (in recommended learning order)
STREAM_SOURCES = 01_stream_server.c 01_stream_client.c
DGRAM_SOURCES = 02_dgram_server.c 02_dgram_client.c
INTERACTIVE_SOURCES = 03_interactive_server.c 03_interactive_client.c
ABSTRACT_SOURCES = 04_abstract_server.c 04_abstract_client.c
CRED_SOURCES = 05_cred_server.c 05_cred_client.c

# Executables (in recommended learning order)
STREAM_TARGETS = 01_stream_server 01_stream_client
DGRAM_TARGETS = 02_dgram_server 02_dgram_client
INTERACTIVE_TARGETS = 03_interactive_server 03_interactive_client
ABSTRACT_TARGETS = 04_abstract_server 04_abstract_client
CRED_TARGETS = 05_cred_server 05_cred_client

ALL_TARGETS = $(STREAM_TARGETS) $(DGRAM_TARGETS) $(INTERACTIVE_TARGETS) $(ABSTRACT_TARGETS) $(CRED_TARGETS)

# Default target
all: $(ALL_TARGETS)

# 1. Stream Socket Examples
01_stream_server: 01_stream_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

01_stream_client: 01_stream_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 2. Datagram Socket Examples
02_dgram_server: 02_dgram_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

02_dgram_client: 02_dgram_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 3. Interactive Stream Socket Examples
03_interactive_server: 03_interactive_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

03_interactive_client: 03_interactive_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 4. Abstract Namespace Examples
04_abstract_server: 04_abstract_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

04_abstract_client: 04_abstract_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 5. Credential Passing Examples
05_cred_server: 05_cred_server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

05_cred_client: 05_cred_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Test targets
test-stream: unix_server unix_client
	@echo "=== Testing Stream Sockets ==="
	@echo "Start server in one terminal: ./unix_server"
	@echo "Run client in another: ./unix_client \"Test message\""

test-dgram: unix_dgram_server unix_dgram_client
	@echo "=== Testing Datagram Sockets ==="
	@echo "Start server in one terminal: ./unix_dgram_server"
	@echo "Run client in another: ./unix_dgram_client \"Test datagram\""

test-cred: unix_cred_server unix_cred_client
	@echo "=== Testing Credential Passing ==="
	@echo "Start server in one terminal: ./unix_cred_server"
	@echo "Run client in another: ./unix_cred_client \"Test with credentials\""

# Demo script
demo:
	@echo "=== Unix Domain Socket Demo ==="
	@echo "This demonstrates various Unix domain socket features:"
	@echo ""
	@echo "1. Stream sockets (SOCK_STREAM):"
	@echo "   - Reliable, ordered, connection-based"
	@echo "   - Use: ./unix_server & ./unix_client \"Hello\""
	@echo ""
	@echo "2. Datagram sockets (SOCK_DGRAM):"
	@echo "   - Connectionless, message boundaries preserved"
	@echo "   - Use: ./unix_dgram_server & ./unix_dgram_client \"Hello\""
	@echo ""
	@echo "3. Credential passing (SCM_CREDENTIALS):"
	@echo "   - Process authentication and authorization"
	@echo "   - Use: ./unix_cred_server & ./unix_cred_client \"Hello\""

# Performance comparison
benchmark: $(ALL_TARGETS)
	@echo "=== Performance Characteristics ==="
	@echo "Unix domain sockets vs TCP sockets:"
	@echo "- Lower latency (no network stack)"
	@echo "- Higher throughput (kernel optimization)"
	@echo "- Zero copy in some cases"
	@echo "- Filesystem-based access control"

# Clean up
clean:
	rm -f $(ALL_TARGETS)
	rm -f *.o
	rm -f /tmp/unix_ipc_socket
	rm -f /tmp/unix_dgram_server
	rm -f /tmp/unix_dgram_client
	rm -f /tmp/unix_cred_socket
	@echo "Cleaned executables, object files, and socket files"

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build all examples"
	@echo "  clean        - Remove executables and socket files"
	@echo "  demo         - Show usage examples"
	@echo "  test-stream  - Instructions for stream socket testing"
	@echo "  test-dgram   - Instructions for datagram socket testing"
	@echo "  test-cred    - Instructions for credential testing"
	@echo "  benchmark    - Show performance information"

.PHONY: all clean demo test-stream test-dgram test-cred benchmark help