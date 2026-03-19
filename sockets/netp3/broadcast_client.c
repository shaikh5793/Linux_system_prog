/*
 * UDP Broadcast Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UDP broadcast client
 * 
 * Receives broadcast messages from UDP broadcast server.
 * Demonstrates client-side broadcast reception.
 * 
 * Key concepts:
 * 
 * 1. Broadcast reception:
 *    - Bind to specific port to receive broadcasts
 *    - All clients on network receive same message
 *    - No explicit connection to server needed
 * 
 * 2. Client flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ socket() → bind(port) → recvfrom() → display message → repeat       │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * Assumptions:
 *   - Listens on port specified by command line
 *   - Receives broadcast messages from any server
 *   - Displays received messages with timestamp
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

static int client_running = 1;

void signal_handler(int sig) {
    (void)sig;
    printf("\nStopping broadcast client...\n");
    client_running = 0;
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in client_addr, sender_addr;
    socklen_t sender_len;
    char buffer[BUFFER_SIZE];
    int port;
    ssize_t bytes_received;
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("Example: %s 8001\n", argv[0]);
        exit(1);
    }
    
    port = atoi(argv[1]);
    signal(SIGINT, signal_handler);
    
    /*
     * Create UDP socket
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(1);
    }
    
    /*
     * Configure client address for binding
     */
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(port);
    
    /*
     * Bind to port to receive broadcasts
     */
    if (bind(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        perror("bind failed");
        exit(2);
    }
    
    printf("UDP broadcast client listening on port %d\n", port);
    printf("Waiting for broadcast messages (Ctrl+C to stop)\n");
    
    /*
     * Receive broadcast messages
     */
    sender_len = sizeof(sender_addr);
    while (client_running) {
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr*)&sender_addr, &sender_len);
        
        if (bytes_received < 0) {
            if (client_running) {
                perror("recvfrom failed");
            }
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Broadcast from %s: %s", 
               inet_ntoa(sender_addr.sin_addr), buffer);
    }
    
    close(sockfd);
    printf("Broadcast client stopped\n");
    return 0;
}
