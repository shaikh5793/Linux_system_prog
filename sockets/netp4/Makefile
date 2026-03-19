# Makefile for Netlink Socket Examples
# Author: Raghu Bharadwaj
# Organization: Techveda
# License: MIT

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 

# Examples
SOURCES = interface_list.c process_monitor.c audit_monitor.c uevent_monitor.c
TARGETS = interface_list process_monitor audit_monitor uevent_monitor

# Default target - build all examples
all: $(TARGETS)

# Build everything
everything: $(TARGETS)

# Example targets
interface_list: interface_list.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

process_monitor: process_monitor.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

audit_monitor: audit_monitor.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

uevent_monitor: uevent_monitor.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Test targets
test: all
	@echo "=== Testing Netlink Examples ==="
	@echo "1. Interface List: ./interface_list"
	@echo "2. Process Monitor: sudo ./process_monitor"
	@echo "3. Audit Monitor: sudo ./audit_monitor"
	@echo "4. Device Events: ./uevent_monitor"

# Usage examples
usage:
	@echo "=== Netlink Socket Examples ==="
	@echo "Learn netlink basics with diverse families:"
	@echo ""
	@echo "1. Network interfaces (NETLINK_ROUTE):"
	@echo "   ./interface_list"
	@echo ""
	@echo "2. Process events (NETLINK_CONNECTOR):"
	@echo "   sudo ./process_monitor"
	@echo ""
	@echo "3. Security events (NETLINK_AUDIT):"
	@echo "   sudo ./audit_monitor"
	@echo ""
	@echo "4. Device events (NETLINK_KOBJECT_UEVENT):"
	@echo "   ./uevent_monitor"
	@echo ""
	@echo "Key learning: Netlink is NOT just for networking!"

# Performance and capabilities info
benchmark:
	@echo "=== Netlink Socket Family Diversity ==="
	@echo "These examples demonstrate different netlink families:"
	@echo ""
	@echo "NETLINK_ROUTE (interface_list):"
	@echo "  - Network interface information"
	@echo "  - Request/response pattern"
	@echo "  - Binary structured data"
	@echo ""
	@echo "NETLINK_CONNECTOR (process_monitor):"
	@echo "  - Process lifecycle events"
	@echo "  - Event subscription pattern"
	@echo "  - Real-time notifications"
	@echo ""
	@echo "NETLINK_AUDIT (audit_monitor):"
	@echo "  - Security audit events"
	@echo "  - Text-based messages"
	@echo "  - Security monitoring"
	@echo ""
	@echo "NETLINK_KOBJECT_UEVENT (uevent_monitor):"
	@echo "  - Hardware/device events"
	@echo "  - Device plug/unplug monitoring"
	@echo "  - System hardware changes"
	@echo ""
	@echo "This shows netlink's versatility beyond networking!"

# Clean up
clean:
	rm -f $(TARGETS)
	rm -f *.o
	@echo "Cleaned executables"

# Help
help:
	@echo "Available targets:"
	@echo "  all             - Build all examples (default)"
	@echo "  everything      - Build all examples"
	@echo "  clean           - Remove executables"
	@echo "  usage           - Show usage examples"
	@echo "  test            - Instructions for testing examples"
	@echo "  benchmark       - Show netlink family diversity"
	@echo ""
	@echo "Examples:"
	@echo "  interface_list   - Network interfaces (NETLINK_ROUTE)"
	@echo "  process_monitor  - Process events (NETLINK_CONNECTOR)" 
	@echo "  audit_monitor    - Security events (NETLINK_AUDIT)"
	@echo "  uevent_monitor   - Device events (NETLINK_KOBJECT_UEVENT)"

.PHONY: all everything test usage benchmark clean help