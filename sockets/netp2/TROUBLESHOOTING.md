# Troubleshooting Guide: TCP Local IPC

**Author:** Raghu Bharadwaj (raghu@techveda.org)  
**Organization:** Techveda (www.techveda.org)  
**License:** MIT License  
**Last Updated:** 02-12-2024

## Common Mistakes

### **1. Port Conflicts (EADDRINUSE)**
- **Impact:** Server fails to start
- **Detection:** `netstat -tuln | grep <port>`
- **Solution:** Use `SO_REUSEADDR`, change port, ensure proper shutdown

### **2. Connection Handling Errors**
- **Impact:** Connection refused or reset by peer
- **Detection:** Monitor logs for connection errors
- **Solution:** Verify server is listening, check firewall rules

### **3. Partial/Interrupted Transfers**
- **Impact:** Data loss or incomplete transmission
- **Detection:** Compare send/receive byte counts
- **Solution:** Implement loop for `send()` and `recv()`

## Debugging Tips

### **Network Analysis**
- Use `tcpdump` to monitor TCP packets on the loopback interface

### **Socket Options Checking**
```bash
ss -tuln | grep LISTEN
```

### **Connection Monitoring**
```bash
watch ss -tnp
```

## Common Pitfalls

### **Incorrect Use of Blocking I/O**
- Implement non-blocking I/O with `fcntl()` for scalability

### **Resource Leak**
- Ensure all sockets are closed properly using `close()`

## System Configuration

### **Adjust TCP Backlog**
```bash
echo 128 > /proc/sys/net/core/somaxconn
```

### **Disable Firewall Temporarily (Development Only)**
```bash
sudo ufw disable
```

## Best Practices

### **Graceful Shutdown**
- Handle termination signals to close sockets cleanly

### **Robust Error Handling**
```c
if (listen(sockfd, SOMAXCONN) == -1) {
    perror("listen error");
    exit(EXIT_FAILURE);
}
```

### **Binary Protocols Support**
- Use length-prefix framing for data transmission

## Real-World Examples

### **Non-blocking Connect Example**
```c
int flags = fcntl(sockfd, F_GETFL, 0);
fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

int result = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
if (result == -1 && errno != EINPROGRESS) {
    perror("connect error");
}
```

## Common Error Messages

### **Error Message:** "Connection refused"
- **Cause:** Server not running or wrong port
- **Solution:** Start server, verify correct port

### **Error Message:** "Connection reset by peer"
- **Cause:** Server unexpectedly closed connection
- **Solution:** Check server logs for crashes
