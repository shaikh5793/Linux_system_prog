/*
 * Basic Unix Domain Socket Server (SOCK_STREAM)
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * BASIC UNIX DOMAIN SOCKET - STREAM SERVER
 * 
 * This example demonstrates the fundamental concepts of Unix domain sockets
 * using SOCK_STREAM (connection-oriented, reliable communication).
 * 
 * 
 * KEY CONCEPTS:
 * 
 * 1. Socket Address Family (AF_UNIX):
 *    - Enables IPC between processes on the same machine
 *    - Uses filesystem paths as addresses (visible with 'ls')
 *    - Provides better performance than network sockets for local communication
 * 
 * 2. SOCK_STREAM Characteristics:
 *    - Connection-oriented (like TCP but local)
 *    - Reliable, ordered delivery
 *    - Requires listen() and accept() for server
 *    - Bidirectional communication channel
 * 
 * 3. Filesystem Integration:
 *    - Socket creates a special file in the filesystem
 *    - File permissions control access to the socket
 *    - Must unlink() the socket file for cleanup
 *    - Socket file persists until explicitly removed
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>  // socket(), bind(), listen(), accept(), recv(), send()
#include <sys/un.h>      // struct sockaddr_un, AF_UNIX
#include <errno.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/basic_stream_socket"
#define BUFFER_SIZE 1024

static int listen_fd = -1;

void cleanup_and_exit(int sig) {
    (void)sig;  // Suppress unused parameter warning
    
    printf("\n=== Server Shutdown ===\n");
    
    // Close server socket if open
    if (listen_fd != -1) {
        close(listen_fd);
        printf("Listen socket closed\n");
    }
    
    // Remove socket file from filesystem
    if (unlink(SOCKET_PATH) == 0) {
        printf("Socket file removed: %s\n", SOCKET_PATH);
    }
    
    printf("Basic stream server shutdown complete\n");
    exit(0);
}

int main() {
    struct sockaddr_un server_addr;
    int conn_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    
    // Setup signal handlers for graceful shutdown
    signal(SIGINT, cleanup_and_exit);   // Ctrl+C
    signal(SIGTERM, cleanup_and_exit);  // kill command
    
    printf("=== Basic Unix Domain Socket Server ===\n");
    printf("Socket type: SOCK_STREAM (connection-oriented)\n");
    printf("Address family: AF_UNIX (local IPC)\n\n");
    
    /*
     * STEP 1: Create Unix domain socket
     * 
     * socket(AF_UNIX, SOCK_STREAM, 0)
     * - AF_UNIX: Unix domain socket family for local IPC
     * - SOCK_STREAM: Reliable, connection-oriented communication
     * - 0: Protocol (ignored for Unix sockets)
     */
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Socket created (fd=%d)\n", listen_fd);
    
    /*
     * STEP 2: Setup server address structure
     * 
     * struct sockaddr_un contains:
     * - sun_family: Address family (AF_UNIX)
     * - sun_path: Filesystem path for the socket
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Remove any existing socket file to avoid EADDRINUSE
    unlink(SOCKET_PATH);
    
    /*
     * STEP 3: Bind socket to filesystem path
     * 
     * This creates the socket file in the filesystem.
     * The file will be visible with 'ls -la /tmp/basic_stream_socket'
     */
    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: bind() failed");
        close(listen_fd);
        exit(1);
    }
    printf("✓ Socket bound to path: %s\n", SOCKET_PATH);
    
    /*
     * STEP 4: Start listening for connections
     * 
     * listen(fd, backlog)
     * - backlog=5: Maximum number of pending connections
     */
    if (listen(listen_fd, 5) == -1) {
        perror("ERROR: listen() failed");
        close(listen_fd);
        unlink(SOCKET_PATH);
        exit(1);
    }
    printf("✓ Server listening (backlog=5)\n");
    printf("\nServer ready! Press Ctrl+C to stop\n");
    printf("Waiting for client connections...\n\n");
    
    /*
     * STEP 5: Accept and handle client connections
     * 
     * This is a simple server that handles one client at a time.
     * Each client connection is processed completely before
     * accepting the next one.
     */
    while (1) {
        /*
         * accept() blocks until a client connects
         * Returns a new file descriptor for communication with the client
         */
        conn_fd = accept(listen_fd, NULL, NULL);
        if (conn_fd == -1) {
            if (errno == EINTR) continue;  // Interrupted by signal
            perror("ERROR: accept() failed");
            continue;
        }
        
        printf("=== New Client Connected ===\n");
        printf("Connection fd: %d\n", conn_fd);
        
        /*
         * Receive message from client
         * 
         * IMPORTANT: SOCK_STREAM is a BYTE STREAM - no message boundaries!
         * - recv() may return partial data from a single send()
         * - Multiple send() calls may be combined into one recv()
         * - Application must handle message boundaries itself
         * 
         * This simple example assumes small messages that fit in one recv(),
         * but real applications need proper message framing.
         */
        bytes_received = recv(conn_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("ERROR: recv() failed");
        } else if (bytes_received == 0) {
            printf("Client disconnected (graceful close)\n");
        } else {
            // Null-terminate the received data
            buffer[bytes_received] = '\0';
            printf("Received (%zd bytes): %s\n", bytes_received, buffer);
            printf("NOTE: This might be partial data! SOCK_STREAM has no message boundaries.\n");
            
            /*
             * Send response back to client
             * send() is used for stream sockets (SOCK_STREAM)
             */
            char response[BUFFER_SIZE + 100];  // Increased buffer size for safety
            snprintf(response, sizeof(response), 
                    "Server processed: %s [%zd bytes received]", 
                    buffer, bytes_received);
            
            ssize_t bytes_sent = send(conn_fd, response, strlen(response), 0);
            if (bytes_sent == -1) {
                perror("ERROR: send() failed");
            } else {
                printf("Sent response (%zd bytes)\n", bytes_sent);
            }
        }
        
        // Close client connection
        close(conn_fd);
        printf("Connection closed\n\n");
        printf("Waiting for next client...\n");
    }
    
    // This point is never reached in normal operation
    cleanup_and_exit(0);
    return 0;
}
