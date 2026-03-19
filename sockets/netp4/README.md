# Netlink Socket Programming Examples

Learn netlink socket programming with diverse kernel families - showing that netlink is much more than just networking!

## Attribution

**Author:** Raghu Bharadwaj (raghu@techveda.org)  
**Copyright:** 2024 Techveda (www.techveda.org)  
**License:** MIT License

## Overview

This directory contains simple, focused examples demonstrating netlink socket programming with **different kernel families**. These examples are designed for userspace programmers learning the fundamental concept that **netlink is a general kernel-userspace IPC mechanism**, not just for networking.

## Why These Examples?

Many netlink tutorials only show `NETLINK_ROUTE`, giving the impression that netlink is networking-only. These examples demonstrate netlink's true diversity by using **four different families** for completely different purposes.

## Examples in this directory

**`interface_list.c`** - **NETLINK_ROUTE** family
- Lists network interfaces 
- Basic request/response pattern
- Binary structured data parsing
- **Purpose:** Network configuration

**`process_monitor.c`** - **NETLINK_CONNECTOR** family  
- Monitors process events (fork, exec, exit)
- Event subscription pattern
- Real-time notifications
- **Purpose:** Process lifecycle monitoring

**`audit_monitor.c`** - **NETLINK_AUDIT** family
- Monitors security audit events
- Text-based message format  
- Security event streams
- **Purpose:** Security monitoring

**`uevent_monitor.c`** - **NETLINK_KOBJECT_UEVENT** family
- Monitors hardware device events
- Device plug/unplug detection
- Text key=value format
- **Purpose:** Hardware monitoring

## Key Learning Points

### Netlink Diversity
- **4 different families** = 4 different kernel subsystems
- **Not just networking** - process, security, hardware monitoring
- **Various data formats** - binary, text, structured
- **Different patterns** - request/response vs event streams

### Core Concepts
- **Direct kernel communication** via socket IPC
- **Family-specific protocols** for different purposes  
- **Multicast vs unicast** communication patterns
- **Structured message format** with headers and payloads

## Quick Start

### 1. Build Examples
```bash
make
```

### 2. Try Each Family

```bash
# Network interfaces (no privileges needed)
./interface_list

# Process events (requires root)
sudo ./process_monitor

# Security events (requires root) 
sudo ./audit_monitor

# Device events (no privileges needed)
./uevent_monitor
```

### 3. Generate Events to See

**For process monitor:**
```bash
# In another terminal while process_monitor runs:
ps
ls
cat /proc/version
```

**For audit monitor:**
```bash
# In another terminal while audit_monitor runs:
sudo su
sudo ls
```

**For device monitor:**
```bash
# Plug/unplug USB devices
# Or simulate with:
echo "add" | sudo tee /sys/power/state
```

## Understanding the Code

Each example follows the same basic pattern:

1. **Create socket:** `socket(AF_NETLINK, SOCK_RAW, FAMILY)`
2. **Bind address:** Set process ID and groups
3. **Send/Subscribe:** Request data or subscribe to events  
4. **Receive/Parse:** Handle responses or event streams

The **key difference** is the `FAMILY` parameter - each family connects to a different kernel subsystem!

## Netlink Families Explained

| Family | Purpose | Data Format | Pattern | Privileges |
|--------|---------|-------------|---------|------------|
| `NETLINK_ROUTE` | Network config | Binary | Request/Response | None |
| `NETLINK_CONNECTOR` | Process events | Binary | Event stream | Root |
| `NETLINK_AUDIT` | Security events | Text | Event stream | Root |
| `NETLINK_KOBJECT_UEVENT` | Device events | Text | Event stream | None |

## Building and Testing

```bash
make             # Build all examples
make clean       # Remove executables
make usage       # Show usage examples
make test        # Instructions for testing
make help        # All available targets
```

## Common Netlink Families

Beyond these examples, Linux provides many netlink families:

- **NETLINK_ROUTE** - Network interfaces, routing, neighbors
- **NETLINK_CONNECTOR** - Process events, custom connectors
- **NETLINK_AUDIT** - Security audit events  
- **NETLINK_KOBJECT_UEVENT** - Device/hardware events
- **NETLINK_GENERIC** - Custom protocols framework
- **NETLINK_SOCK_DIAG** - Socket diagnostics
- **NETLINK_CRYPTO** - Crypto API events
- **NETLINK_SELINUX** - SELinux events

## Why Netlink Matters

### Traditional Approach (Filesystem)
```bash
cat /proc/net/dev        # Network stats
cat /proc/meminfo        # Memory info
cat /proc/stat           # CPU stats
```
- **Multiple file reads**
- **Text parsing overhead** 
- **Potential race conditions**
- **Cached/stale data**

### Netlink Approach (Socket IPC)
```c
socket(AF_NETLINK, SOCK_RAW, family)  // Direct kernel access
```
- **Single socket operation**
- **Binary structured data**
- **Real-time kernel state**
- **Atomic data snapshots**

## Security Considerations

- **Process monitoring** requires root (sensitive data)
- **Audit events** require root (security data)  
- **Network queries** generally work without privileges
- **Device events** accessible to all users

## Prerequisites

- Linux system with netlink support
- GCC compiler with C99 support  
- Root privileges for some examples
- Active network interfaces for meaningful output
- **Audit daemon (`auditd`) for audit_monitor example**

## Troubleshooting

**Permission denied:** Use `sudo` for process and audit monitors  
**No events:** Generate activity (run commands, plug devices)  
**Compilation errors:** Install build tools (`build-essential` on Ubuntu)  
**Empty output:** Check if interfaces exist / audit daemon running
**Audit monitor not receiving events:** Install and start audit daemon:
```bash
sudo apt install auditd
sudo systemctl start auditd
sudo systemctl enable auditd
```

## Next Steps

After understanding these basics:
1. Explore custom netlink families with `NETLINK_GENERIC`
2. Learn about netlink attributes and nested data
3. Study kernel netlink implementation

## The Big Picture

**Netlink is Linux's general kernel-userspace IPC mechanism.** It's used for:
- Network configuration (most common)
- Process monitoring
- Security auditing  
- Hardware events
- Custom kernel modules
- Real-time kernel notifications

These examples show you can communicate with almost any kernel subsystem through the right netlink family!
