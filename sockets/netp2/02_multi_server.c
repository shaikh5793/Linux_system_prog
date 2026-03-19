/*
 * Concurrent TCP Server
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

static int listen_fd = -1;
static int active_clients = 0;

void handle_sigchld(int sig) {
	(void)sig;  /* Suppress unused parameter warning */
    int status;
    pid_t pid;
    
    // Reap all available zombie children
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        active_clients--;
        printf("Child process %d terminated (active clients: %d)\n", pid, active_clients);
    }
}

void cleanup_and_exit(int sig) {
	(void)sig;  /* Suppress unused parameter warning */
    if (listen_fd != -1) {
        close(listen_fd);
    }
    printf("\nMulti-client TCP server shutdown complete\n");
    exit(0);
}

void handle_client(int conn_fd, struct sockaddr_in *client_addr) {
    char buffer[BUFFER_SIZE];
    char client_ip[INET_ADDRSTRLEN];
    ssize_t bytes_received;
    pid_t my_pid = getpid();
    
    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, INET_ADDRSTRLEN);
    
    printf("[PID %d] Handling client %s:%d\n", 
           my_pid, client_ip, ntohs(client_addr->sin_port));
    
    // Send welcome message
    char welcome[BUFFER_SIZE];
    snprintf(welcome, sizeof(welcome), 
            "Welcome! You are being served by process %d", my_pid);
    
    if (send(conn_fd, welcome, strlen(welcome), 0) == -1) {
        perror("welcome send failed");
        close(conn_fd);
        exit(1);
    }
    
    // Handle client messages
    while (1) {
        bytes_received = recv(conn_fd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("[PID %d] Client %s disconnected\n", my_pid, client_ip);
            } else {
                perror("recv failed");
            }
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("[PID %d] Received from %s: %s\n", my_pid, client_ip, buffer);
        
        // Simulate some processing time
        sleep(1);
        
        // Send response with process info
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response),
                "[Worker %d] Processed: %.992s", my_pid, buffer);
        
        if (send(conn_fd, response, strlen(response), 0) == -1) {
            perror("send failed");
            break;
        }
        
        printf("[PID %d] Response sent to %s\n", my_pid, client_ip);
    }
    
    close(conn_fd);
    printf("[PID %d] Client handler terminating\n", my_pid);
    exit(0);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    int conn_fd;
    socklen_t client_len;
    int port;
    int optval = 1;
    pid_t child_pid;
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("Example: %s 8081\n", argv[0]);
        exit(1);
    }
    
    port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Use 1-65535\n");
        exit(1);
    }
    
    // Setup signal handlers
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    signal(SIGCHLD, handle_sigchld);
    
    // Create TCP socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket creation failed");
        exit(1);
    }
    
    // Set socket options
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt failed");
        close(listen_fd);
        exit(1);
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);
    
    // Bind and listen
    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(listen_fd);
        exit(1);
    }
    
    if (listen(listen_fd, MAX_CLIENTS) == -1) {
        perror("listen failed");
        close(listen_fd);
        exit(1);
    }
    
    printf("Multi-Client TCP Server listening on 127.0.0.1:%d\n", port);
    printf("Master PID: %d\n", getpid());
    printf("Max concurrent clients: %d\n", MAX_CLIENTS);
    printf("Press Ctrl+C to stop server\n\n");
    
    // Main accept loop
    while (1) {
        client_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (conn_fd == -1) {
            if (errno == EINTR) continue;
            perror("accept failed");
            continue;
        }
        
        // Check if we've reached the client limit
        if (active_clients >= MAX_CLIENTS) {
            printf("Maximum clients reached. Rejecting connection.\n");
            close(conn_fd);
            continue;
        }
        
        // Fork a child process to handle the client
        child_pid = fork();
        
        if (child_pid == -1) {
            perror("fork failed");
            close(conn_fd);
            continue;
        }
        
        if (child_pid == 0) {
            // Child process - handle the client
            close(listen_fd);  // Child doesn't need the listening socket
            handle_client(conn_fd, &client_addr);
            // handle_client() will exit the child process
        } else {
            // Parent process - continue accepting connections
            close(conn_fd);  // Parent doesn't need the client socket
            active_clients++;
            printf("Forked child %d for new client (active clients: %d)\n", 
                   child_pid, active_clients);
        }
    }
    
    cleanup_and_exit(0);
    return 0;
}
