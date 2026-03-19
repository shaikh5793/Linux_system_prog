/*
 * UDP Broadcast Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UDP broadcast server
 * 
 * Demonstrates UDP broadcasting for one-to-many communication.
 * Sends periodic broadcast messages to all clients on network.
 * 
 * Key concepts:
 * 
 * 1. UDP broadcasting:
 *    - One server sends to multiple clients simultaneously
 *    - Uses broadcast address (255.255.255.255)
 *    - No individual client connections needed
 * 
 * 2. Broadcast flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ socket() → setsockopt(SO_BROADCAST) → sendto(broadcast_addr)        │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. Socket options:
 *    - SO_BROADCAST: Enables broadcast transmission
 *    - Required for sending to broadcast addresses
 * 
 * Assumptions:
 *   - Broadcasts on port specified by command line
 *   - Sends message every 5 seconds
 *   - Uses local broadcast address
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define BROADCAST_INTERVAL 5

static int server_running = 1;

void signal_handler(int sig) {
    (void)sig;
    printf("\nStopping broadcast server...\n");
    server_running = 0;
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in broadcast_addr;
    int port;
    int broadcast_enable = 1;
    char message[BUFFER_SIZE];
    int message_count = 0;
    
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
     * Enable broadcast option
     * Allows sending to broadcast addresses
     */
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, 
                   &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        perror("setsockopt SO_BROADCAST failed");
        exit(2);
    }
    
    /*
     * Configure broadcast address
     */
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(port);
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
    
    printf("UDP broadcast server starting on port %d\n", port);
    printf("Broadcasting every %d seconds (Ctrl+C to stop)\n", BROADCAST_INTERVAL);
    
    /*
     * Broadcast loop
     */
    while (server_running) {
        /*
         * Create broadcast message with timestamp
         */
        time_t current_time = time(NULL);
        snprintf(message, BUFFER_SIZE, 
                "Broadcast #%d from server at %s", 
                ++message_count, ctime(&current_time));
        
        /*
         * Send broadcast message
         */
        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr*)&broadcast_addr, 
                   sizeof(broadcast_addr)) < 0) {
            perror("sendto failed");
        } else {
            printf("Sent broadcast #%d\n", message_count);
        }
        
        sleep(BROADCAST_INTERVAL);
    }
    
    close(sockfd);
    printf("Broadcast server stopped\n");
    return 0;
}
