/*
 * Unix Domain Socket Datagram Server (SOCK_DGRAM)
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UNIX DOMAIN SOCKET - DATAGRAM SERVER
 * 
 * This example demonstrates Unix domain sockets using SOCK_DGRAM
 * (connectionless, message-oriented communication).
 * 
 * KEY CONCEPTS:
 * 
 * 1. SOCK_DGRAM Characteristics:
 *    - Connectionless (no listen/accept/connect)
 *    - Message boundaries preserved
 *    - Each message is independent
 *    - Lower overhead than SOCK_STREAM
 * 
 * 2. Datagram Communication Pattern:
 *    ┌─────────┐    sendto()     ┌─────────┐
 *    │ Client  │ ──────────────► │ Server  │
 *    │         │                 │ (bound) │
 *    │(address)│ ◄────────────── │         │
 *    └─────────┘    sendto()     └─────────┘
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>

#define SERVER_SOCKET_PATH "/tmp/datagram_server_socket"
#define BUFFER_SIZE 1024

static int server_fd = -1;

void cleanup_and_exit(int sig) {
    (void)sig;
    printf("\n=== Datagram Server Shutdown ===\n");
    if (server_fd != -1) {
        close(server_fd);
        printf("Server socket closed\n");
    }
    if (unlink(SERVER_SOCKET_PATH) == 0) {
        printf("Socket file removed: %s\n", SERVER_SOCKET_PATH);
    }
    printf("Datagram server shutdown complete\n");
    exit(0);
}

int main() {
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int message_count = 0;
    
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    
    printf("=== Unix Domain Socket Datagram Server ===\n");
    printf("Socket type: SOCK_DGRAM (connectionless)\n\n");
    
    /*
     * Create Unix domain datagram socket
     * Key differences from SOCK_STREAM:
     * - SOCK_DGRAM: Message-oriented, connectionless
     * - No need for listen() or accept()
     * - Use recvfrom() and sendto() instead of recv() and send()
     */
    server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Datagram socket created (fd=%d)\n", server_fd);
    
    unlink(SERVER_SOCKET_PATH);
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, 
            sizeof(server_addr.sun_path) - 1);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: bind() failed");
        close(server_fd);
        exit(1);
    }
    printf("✓ Socket bound to path: %s\n", SERVER_SOCKET_PATH);
    printf("\nServer ready! Press Ctrl+C to stop\n");
    printf("Waiting for datagram messages...\n\n");
    
    /*
     * Main server loop - receive and respond to messages
     * Unlike SOCK_STREAM, there's no accept() call.
     * We directly receive messages from any client.
     */
    while (1) {
        client_len = sizeof(client_addr);
        
        /*
         * recvfrom() receives a single message (datagram)
         * 
         * CRITICAL DIFFERENCE FROM SOCK_STREAM:
         * - Each recvfrom() call receives EXACTLY ONE complete message
         * - Message boundaries are ALWAYS preserved
         * - If sender sends "Hello", receiver gets exactly "Hello"
         * - No partial messages, no message combining
         * - Each send() corresponds to exactly one recv()
         * 
         * This is the KEY advantage of SOCK_DGRAM for message-based protocols!
         */
        bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                 (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes_received == -1) {
            if (errno == EINTR) continue;
            perror("ERROR: recvfrom() failed");
            continue;
        }
        
        buffer[bytes_received] = '\0';
        message_count++;
        
        printf("=== Message #%d ===\n", message_count);
        printf("Received (%zd bytes): %s\n", bytes_received, buffer);
        
        if (client_addr.sun_path[0] != '\0') {
            printf("From client: %s\n", client_addr.sun_path);
        } else {
            printf("From client: (anonymous or abstract namespace)\n");
        }
        
        /*
         * Send response back to client
         * sendto() sends a message to the specified address.
         */
        char response[BUFFER_SIZE + 100];  // Increased buffer size for safety
        snprintf(response, sizeof(response), 
                "Server ACK #%d: Processed '%s' (%zd bytes)", 
                message_count, buffer, bytes_received);
        
        ssize_t bytes_sent = sendto(server_fd, response, strlen(response), 0,
                                   (struct sockaddr*)&client_addr, client_len);
        
        if (bytes_sent == -1) {
            perror("ERROR: sendto() failed");
        } else {
            printf("Sent response (%zd bytes)\n", bytes_sent);
        }
        
        printf("─────────────────────────\n");
    }
    
    cleanup_and_exit(0);
    return 0;
}
