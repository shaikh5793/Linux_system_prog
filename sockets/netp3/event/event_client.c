/*
 * UDP Event Notification Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UDP event notification client
 * 
 * Connects to event server and registers for event notifications.
 * Receives and displays events broadcast by the server.
 * 
 * Key concepts:
 * 
 * 1. Event client flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ Connect → register → listen for events → process events → cleanup   │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 2. Event processing:
 *    - Parse "event:type:data" format
 *    - Filter events by type (if needed)
 *    - Display event information
 * 
 * Assumptions:
 *   - Connects to server address and port from command line
 *   - Registers for all event types
 *   - Runs until user interrupts (Ctrl+C)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

static int client_running = 1;

void signal_handler(int sig) {
    (void)sig;
    printf("\nShutting down event client...\n");
    client_running = 0;
}

/*
 * Parse and display event message
 */
void process_event(const char* message) {
    if (strncmp(message, "event:", 6) == 0) {
        /*
         * Parse event format: "event:type:data"
         */
        char event_copy[BUFFER_SIZE];
        strncpy(event_copy, message, BUFFER_SIZE - 1);
        event_copy[BUFFER_SIZE - 1] = '\0';
        
        char* token = strtok(event_copy, ":");
        if (token && strcmp(token, "event") == 0) {
            char* event_type = strtok(NULL, ":");
            char* event_data = strtok(NULL, ":");
            
            if (event_type && event_data) {
                printf("Event received - Type: %s, Data: %s\n", event_type, event_data);
            }
        }
    } else {
        printf("Server message: %s\n", message);
    }
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, local_addr;
    char buffer[BUFFER_SIZE];
    char *server_ip;
    int server_port;
    ssize_t bytes_received;
    socklen_t addr_len;
    
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        printf("Example: %s 127.0.0.1 8002\n", argv[0]);
        exit(1);
    }
    
    server_ip = argv[1];
    server_port = atoi(argv[2]);
    
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
     * Configure server address
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid server IP address\n");
        close(sockfd);
        exit(1);
    }
    
    /*
     * Bind to local address for receiving responses
     */
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = 0; /* let system choose port */
    
    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(2);
    }
    
    /*
     * Get assigned local port for display
     */
    addr_len = sizeof(local_addr);
    getsockname(sockfd, (struct sockaddr*)&local_addr, &addr_len);
    
    printf("Event client started on port %d\n", ntohs(local_addr.sin_port));
    printf("Connecting to event server %s:%d\n", server_ip, server_port);
    
    /*
     * Register with event server
     */
    const char* register_msg = "register";
    if (sendto(sockfd, register_msg, strlen(register_msg), 0,
              (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("failed to register with server");
        close(sockfd);
        exit(3);
    }
    
    printf("Registration sent to server\n");
    printf("Waiting for events (Ctrl+C to stop)...\n");
    
    /*
     * Event listening loop
     */
    while (client_running) {
        /*
         * Wait for events with timeout
         */
        fd_set read_fds;
        struct timeval timeout = {1, 0}; /* 1 second timeout */
        
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        
        int select_result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (select_result > 0 && FD_ISSET(sockfd, &read_fds)) {
            /*
             * Receive event or response
             */
            bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
            
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                process_event(buffer);
            }
        } else if (select_result < 0) {
            if (client_running) {
                perror("select failed");
                break;
            }
        }
        /* timeout - continue loop for periodic check */
    }
    
    /*
     * Unregister from server before exit
     */
    const char* unregister_msg = "unregister";
    sendto(sockfd, unregister_msg, strlen(unregister_msg), 0,
          (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    printf("Unregistered from server\n");
    close(sockfd);
    return 0;
}
