/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MESSAGE_COUNT 5

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    const char *server_ip, *client_name;
    int port;
    ssize_t bytes_received;
    pid_t my_pid = getpid();
    
    if (argc != 4) {
        printf("Usage: %s <server_ip> <port> <client_name>\n", argv[0]);
        printf("Example: %s 127.0.0.1 8081 \"Client-A\"\n", argv[0]);
        exit(1);
    }
    
    server_ip = argv[1];
    port = atoi(argv[2]);
    client_name = argv[3];
    
    printf("Multi-Client TCP Client: %s (PID: %d)\n", client_name, my_pid);
    printf("Connecting to %s:%d\n", server_ip, port);
    
    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket creation failed");
        exit(1);
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid IP address: %s\n", server_ip);
        close(client_fd);
        exit(1);
    }
    
    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
        printf("Make sure multi-server is running on %s:%d\n", server_ip, port);
        close(client_fd);
        exit(1);
    }
    
    printf("Connected successfully\n");
    
    // Receive welcome message
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Server welcome: %s\n\n", buffer);
    }
    
    // Send multiple messages to test concurrent processing
    for (int i = 1; i <= MESSAGE_COUNT; i++) {
        char message[BUFFER_SIZE];
        snprintf(message, sizeof(message), "%s message %d (PID %d)", client_name, i, my_pid);
        
        printf("Sending: %s\n", message);
        
        if (send(client_fd, message, strlen(message), 0) == -1) {
            perror("send failed");
            break;
        }
        
        // Receive response
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Response: %s\n", buffer);
        
        // Small delay between messages
        sleep(2);
    }
    
    printf("\nSent %d messages. Entering interactive mode.\n", MESSAGE_COUNT);
    printf("Type messages to send, or 'quit' to exit.\n");
    
    // Interactive mode
    while (1) {
        printf("[%s] Enter message: ", client_name);
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
        
        // Prefix with client name
        char prefixed_message[BUFFER_SIZE];
        snprintf(prefixed_message, sizeof(prefixed_message), "%.500s: %.500s", client_name, buffer);
        
        if (send(client_fd, prefixed_message, strlen(prefixed_message), 0) == -1) {
            perror("send failed");
            break;
        }
        
        // Receive response
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Server: %s\n", buffer);
    }
    
    close(client_fd);
    printf("Client %s (PID %d) disconnected\n", client_name, my_pid);
    return 0;
}
