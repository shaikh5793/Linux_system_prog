/*
 * Threaded TCP Socket Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * threaded tcp socket - client side
 * 
 * this is a simple tcp client that connects to the threaded server.
 * demonstrates how clients interact with multi-threaded servers.
 * 
 * key concepts:
 * 
 * 1. client perspective:
 *    - client doesn't need to know server is threaded
 *    - connection and communication work the same way
 *    - server handles multiple clients simultaneously
 * 
 * 2. connection flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ client                          server                              │
 *    │  ├── socket()                   ├── accept() creates new thread     │
 *    │  ├── connect() ──────────────── ├── thread handles this client      │
 *    │  ├── send/recv ←─────────────── ├── while other threads handle      │
 *    │  └── close()                    └── other clients simultaneously    │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * assumptions:
 *   - connects to localhost on specified port
 *   - sends interactive messages to server
 *   - displays server responses with thread information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    int port;
    char *server_ip;
    ssize_t bytes_received;
    
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        printf("Example: %s 127.0.0.1 8082\n", argv[0]);
        exit(1);
    }
    
    server_ip = argv[1];
    port = atoi(argv[2]);
    
    /*
     * socket creation
     */
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket creation failed");
        exit(1);
    }
    
    /*
     * server address configuration
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid server IP address\n");
        close(client_fd);
        exit(1);
    }
    
    /*
     * connect to server
     */
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connection failed");
        close(client_fd);
        exit(1);
    }
    
    printf("Connected to threaded server %s:%d\n", server_ip, port);
    
    /*
     * receive welcome message
     */
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Welcome message: %s\n", buffer);
    }
    
    /*
     * interactive communication loop
     */
    printf("Enter messages (type 'quit' to exit):\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }
        
        if (strncmp(message, "quit", 4) == 0) {
            break;
        }
        
        /*
         * send message to server
         */
        if (send(client_fd, message, strlen(message), 0) == -1) {
            perror("send failed");
            break;
        }
        
        /*
         * receive response from server
         */
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Server: %s", buffer);
    }
    
    /*
     * cleanup
     */
    close(client_fd);
    printf("Disconnected from server\n");
    return 0;
}
