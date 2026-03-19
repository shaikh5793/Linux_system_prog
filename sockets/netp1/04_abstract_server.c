/*
 * Unix Domain Socket Abstract Namespace Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UNIX DOMAIN SOCKET - ABSTRACT NAMESPACE SERVER
 * 
 * This example demonstrates Unix domain sockets using the abstract namespace,
 * a Linux-specific feature that provides filesystem-independent socket addressing.
 * 
 * KEY CONCEPTS:
 * 
 * 1. Abstract Namespace:
 *    - Socket addresses beginning with null byte (\0) use abstract namespace
 *    - No filesystem entries are created - sockets exist only in kernel memory
 *    - Automatically cleaned up when the last reference is closed
 *    - Namespace is separate from filesystem paths
 * 
 * 2. Advantages over Filesystem Sockets:
 *    - No file cleanup required (no unlink() needed)
 *    - No filesystem permission issues
 *    - No stale socket files left behind after crashes
 *    - Faster creation/destruction (no filesystem operations)
 *    - Immune to filesystem namespace changes (chroot, containers)
 * 
 * 3. Address Format:
 *    ┌─────────────────────────────────────────────┐
 *    │            struct sockaddr_un               │
 *    ├─────────────────────────────────────────────┤
 *    │ sun_family = AF_UNIX                        │
 *    │ sun_path[0] = '\0'  (null byte indicator)   │
 *    │ sun_path[1..n] = "abstract_socket"          │
 *    └─────────────────────────────────────────────┘
 * 
 * 4. Visibility and Discovery:
 *    - Abstract sockets are visible in /proc/net/unix
 *    - Names shown with @ prefix (e.g., @abstract_socket)
 *    - Use 'ss -x' or 'netstat -x' to see abstract sockets
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>

#define ABSTRACT_SOCKET_NAME "\0abstract_socket_demo"
#define BUFFER_SIZE 1024

static int server_fd = -1;

void cleanup_and_exit(int sig) {
    (void)sig;
    printf("\n=== Abstract Namespace Server Shutdown ===\n");
    
    if (server_fd != -1) {
        close(server_fd);
        printf("Server socket closed\n");
    }
    
    printf("Abstract namespace server shutdown complete\n");
    printf("Note: No filesystem cleanup needed for abstract namespace!\n");
    exit(0);
}

int main() {
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    int client_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int client_count = 0;
    
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    
    printf("=== Unix Domain Socket Abstract Namespace Server ===\n");
    printf("Socket type: SOCK_STREAM (connection-oriented)\n");
    printf("Namespace: Abstract (Linux-specific, filesystem-independent)\n");
    printf("Socket name: @abstract_socket_demo\n\n");
    
    /*
     * Create Unix domain socket for abstract namespace
     */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Abstract namespace socket created (fd=%d)\n", server_fd);
    
    /*
     * Setup server address for abstract namespace
     * 
     * CRITICAL: The first byte of sun_path MUST be '\0' to indicate
     * abstract namespace. This tells the kernel NOT to create a
     * filesystem entry.
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    
    // Copy the abstract socket name (including the leading null byte)
    memcpy(server_addr.sun_path, ABSTRACT_SOCKET_NAME, 
           strlen(ABSTRACT_SOCKET_NAME + 1) + 1);
    
    /*
     * Bind to abstract namespace
     * 
     * Unlike filesystem sockets:
     * - No need to unlink() first
     * - No filesystem permissions to worry about
     * - No file creation/deletion overhead
     */
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: bind() failed");
        close(server_fd);
        exit(1);
    }
    printf("✓ Socket bound to abstract namespace: @abstract_socket_demo\n");
    
    /*
     * Start listening for connections
     */
    if (listen(server_fd, 5) == -1) {
        perror("ERROR: listen() failed");
        close(server_fd);
        exit(1);
    }
    printf("✓ Server listening on abstract namespace\n");
    
    printf("\nServer ready! Press Ctrl+C to stop\n");
    printf("Check with: ss -x | grep '@abstract_socket_demo'\n");
    printf("Waiting for client connections...\n\n");
    
    /*
     * Accept and handle client connections
     */
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd == -1) {
            if (errno == EINTR) continue;
            perror("ERROR: accept() failed");
            continue;
        }
        
        client_count++;
        printf("=== Client #%d Connected ===\n", client_count);
        printf("Client fd: %d\n", client_fd);
        
        /*
         * Display client address information
         * For abstract namespace connections, client address
         * might also be in abstract namespace or unnamed
         */
        if (client_addr.sun_path[0] == '\0' && client_addr.sun_path[1] != '\0') {
            printf("Client address: @%s (abstract namespace)\n", 
                   client_addr.sun_path + 1);
        } else if (client_addr.sun_path[0] != '\0') {
            printf("Client address: %s (filesystem)\n", client_addr.sun_path);
        } else {
            printf("Client address: (unnamed/anonymous)\n");
        }
        
        /*
         * Receive message from client
         */
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("ERROR: recv() failed");
        } else if (bytes_received == 0) {
            printf("Client disconnected (graceful close)\n");
        } else {
            buffer[bytes_received] = '\0';
            printf("Received (%zd bytes): %s\n", bytes_received, buffer);
            
            /*
             * Send response back to client
             */
            char response[BUFFER_SIZE + 100];  // Increased buffer size for safety
            snprintf(response, sizeof(response), 
                    "Abstract namespace server processed: %s [Client #%d, %zd bytes]", 
                    buffer, client_count, bytes_received);
            
            ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
            if (bytes_sent == -1) {
                perror("ERROR: send() failed");
            } else {
                printf("Sent response (%zd bytes)\n", bytes_sent);
            }
        }
        
        /*
         * Close client connection
         */
        close(client_fd);
        printf("Client #%d connection closed\n\n", client_count);
        printf("Waiting for next client...\n");
    }
    
    // This point is never reached in normal operation
    cleanup_and_exit(0);
    return 0;
}
