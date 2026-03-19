/*
 * Basic Unix Domain Socket Client (SOCK_STREAM)
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * BASIC UNIX DOMAIN SOCKET - STREAM CLIENT
 * 
 * This example demonstrates how a client connects to a Unix domain socket
 * server and exchanges messages using SOCK_STREAM.
 * 
 * KEY CONCEPTS:
 * 
 * 1. SOCK_STREAM Characteristics:
 *    - Connection-oriented (requires connect())
 *    - Byte stream (NO message boundaries)
 *    - Reliable, ordered delivery
 *    - send() may send partial data
 *    - recv() may receive partial data
 * 
 * 2. Byte Stream vs Message Boundaries:
 *    - Data sent as continuous stream of bytes
 *    - Application must handle message framing
 *    - Multiple sends may combine into one receive
 *    - One send may split across multiple receives
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/basic_stream_socket"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    const char *message;
    ssize_t bytes_sent, bytes_received;
    
    if (argc != 2) {
        printf("=== Basic Unix Domain Socket Client (STREAM) ===\n");
        printf("Usage: %s <message>\n", argv[0]);
        printf("Example: %s \"Hello Stream Server!\"\n", argv[0]);
        exit(1);
    }
    
    message = argv[1];
    
    printf("=== Basic Unix Domain Socket Client (STREAM) ===\n");
    printf("Socket type: SOCK_STREAM (byte stream, no message boundaries)\n");
    printf("Message to send: %s\n\n", message);
    
    /*
     * Create Unix domain socket
     */
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Stream socket created\n");
    
    /*
     * Setup server address and connect
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    printf("Connecting to server...\n");
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: connect() failed");
        close(client_fd);
        exit(1);
    }
    printf("✓ Connected to stream server\n\n");
    
    /*
     * Demonstrate SOCK_STREAM byte stream behavior
     * Send data as continuous stream
     */
    printf("Sending data as byte stream...\n");
    bytes_sent = send(client_fd, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("ERROR: send() failed");
        close(client_fd);
        exit(1);
    }
    printf("✓ Sent %zd bytes (out of %zu requested)\n", bytes_sent, strlen(message));
    
    if ((size_t)bytes_sent < strlen(message)) {
        printf("WARNING: Partial send! In real apps, must handle remaining %zu bytes\n",
               strlen(message) - bytes_sent);
    }
    
    /*
     * Receive response
     * May get partial data - in real apps need proper message framing
     */
    printf("\nReceiving response...\n");
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1) {
        perror("ERROR: recv() failed");
    } else if (bytes_received == 0) {
        printf("Server closed connection\n");
    } else {
        buffer[bytes_received] = '\0';
        printf("✓ Received %zd bytes: %s\n", bytes_received, buffer);
        printf("NOTE: This could be partial data! SOCK_STREAM = byte stream\n");
    }
    
    close(client_fd);
    printf("\n✓ Stream client finished\n");
    return 0;
}
