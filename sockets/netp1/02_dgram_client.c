/*
 * Unix Domain Socket Datagram Client (SOCK_DGRAM)
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UNIX DOMAIN SOCKET - DATAGRAM CLIENT
 * 
 * This example demonstrates how a client sends messages using SOCK_DGRAM
 * (connectionless communication).
 * 
 * KEY CONCEPTS:
 * 
 * 1. Datagram Client Pattern:
 *    - Create socket with SOCK_DGRAM
 *    - Bind own address (for receiving responses)
 *    - Use sendto() to send to server address
 *    - Use recvfrom() to receive responses
 *    - No connection establishment needed
 * 
 * 2. Message Boundaries:
 *    - Each sendto() sends one complete message
 *    - Each recvfrom() receives one complete message
 *    - Messages don't get fragmented or combined
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCKET_PATH "/tmp/datagram_server_socket"
#define CLIENT_SOCKET_PATH "/tmp/datagram_client_socket"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t server_len;
    char buffer[BUFFER_SIZE];
    const char *message;
    ssize_t bytes_sent, bytes_received;
    
    if (argc != 2) {
        printf("=== Unix Domain Socket Datagram Client ===\n");
        printf("Usage: %s <message>\n", argv[0]);
        printf("Example: %s \"Hello Datagram Server!\"\n", argv[0]);
        exit(1);
    }
    
    message = argv[1];
    
    printf("=== Unix Domain Socket Datagram Client ===\n");
    printf("Socket type: SOCK_DGRAM (message-oriented, boundaries preserved)\n");
    printf("Server address: %s\n", SERVER_SOCKET_PATH);
    printf("Client address: %s\n", CLIENT_SOCKET_PATH);
    printf("Message to send: %s\n\n", message);
    
    /*
     * Create Unix domain datagram socket
     * Same socket type as server: AF_UNIX, SOCK_DGRAM
     */
    client_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Datagram socket created (fd=%d)\n", client_fd);
    
    /*
     * Setup and bind client address
     * IMPORTANT: For datagram sockets, the client should bind to
     * an address so the server can send responses back.
     */
    unlink(CLIENT_SOCKET_PATH);
    
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, CLIENT_SOCKET_PATH, 
            sizeof(client_addr.sun_path) - 1);
    
    if (bind(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
        perror("ERROR: bind() failed");
        close(client_fd);
        exit(1);
    }
    printf("✓ Client socket bound to: %s\n", CLIENT_SOCKET_PATH);
    
    /*
     * Setup server address - where we'll send our message
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, 
            sizeof(server_addr.sun_path) - 1);
    server_len = sizeof(server_addr);
    
    /*
     * Send message to server
     * Note: No connect() call needed for datagram sockets!
     */
    printf("Sending message to server...\n");
    bytes_sent = sendto(client_fd, message, strlen(message), 0,
                       (struct sockaddr*)&server_addr, server_len);
    
    if (bytes_sent == -1) {
        perror("ERROR: sendto() failed");
        printf("\nTroubleshooting:\n");
        printf("- Is the datagram server running?\n");
        printf("- Check if server socket exists: ls -la %s\n", SERVER_SOCKET_PATH);
        close(client_fd);
        unlink(CLIENT_SOCKET_PATH);
        exit(1);
    }
    printf("✓ Sent %zd bytes to server\n", bytes_sent);
    printf("NOTE: With SOCK_DGRAM, this ENTIRE message will be received as ONE unit!\n");
    
    /*
     * Receive response from server
     */
    printf("Waiting for server response...\n");
    bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
    
    if (bytes_received == -1) {
        perror("ERROR: recvfrom() failed");
    } else if (bytes_received == 0) {
        printf("No response from server\n");
    } else {
        buffer[bytes_received] = '\0';
        printf("✓ Received response (%zd bytes): %s\n", bytes_received, buffer);
    }
    
    /*
     * Interactive mode for multiple messages
     */
    printf("\n=== Interactive Mode ===\n");
    printf("Enter messages to send (type 'quit' to exit):\n");
    
    while (1) {
        printf("\nMessage: ");
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
        
        if (strlen(buffer) == 0) {
            continue;
        }
        
        /*
         * Send the new message
         * Each message is independent - no connection state
         */
        bytes_sent = sendto(client_fd, buffer, strlen(buffer), 0,
                           (struct sockaddr*)&server_addr, server_len);
        
        if (bytes_sent == -1) {
            perror("ERROR: sendto() failed");
            break;
        }
        printf("Sent %zd bytes\n", bytes_sent);
        
        // Receive response
        bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Response: %s\n", buffer);
        } else {
            printf("No response received\n");
        }
    }
    
    /*
     * Cleanup
     */
    close(client_fd);
    unlink(CLIENT_SOCKET_PATH);
    
    printf("\n✓ Client finished and cleaned up\n");
    return 0;
}
