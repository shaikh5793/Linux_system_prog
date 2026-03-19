/*
 * Simple UDP Message Queue Client
 * Connects to UDP queue server and sends commands
 * 
 * Operations:
 *   send <message> - Add message to queue
 *   get           - Retrieve next message
 *   status        - Show queue status
 * 
 * Usage:
 *   ./04_queue_client <server_ip> <port> <operation> [message]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int send_command(int sockfd, struct sockaddr_in *server_addr, 
                const char *command, char *response) {
    socklen_t server_len = sizeof(*server_addr);
    
    if (sendto(sockfd, command, strlen(command), 0,
               (struct sockaddr*)server_addr, server_len) == -1) {
        perror("sendto failed");
        return -1;
    }
    
    ssize_t bytes_received = recvfrom(sockfd, response, BUFFER_SIZE - 1, 0,
                                    (struct sockaddr*)server_addr, &server_len);
    
    if (bytes_received == -1) {
        perror("recvfrom failed");
        return -1;
    }
    
    response[bytes_received] = '\0';
    return bytes_received;
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char *server_ip, *operation;
    int port;
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    if (argc < 4) {
        printf("Usage: %s <server_ip> <port> <operation> [message]\n", argv[0]);
        printf("Operations: send, get, status\n");
        printf("Examples:\n");
        printf("  %s 127.0.0.1 8003 send \"Hello World\"\n", argv[0]);
        printf("  %s 127.0.0.1 8003 get\n", argv[0]);
        printf("  %s 127.0.0.1 8003 status\n", argv[0]);
        exit(1);
    }
    
    server_ip = argv[1];
    port = atoi(argv[2]);
    operation = argv[3];
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid IP address\n");
        exit(1);
    }
    
    if (strcmp(operation, "send") == 0) {
        if (argc < 5) {
            printf("Send requires message argument\n");
            exit(1);
        }
        snprintf(command, sizeof(command), "send:%s", argv[4]);
        
    } else if (strcmp(operation, "get") == 0) {
        strcpy(command, "get");
        
    } else if (strcmp(operation, "status") == 0) {
        strcpy(command, "status");
        
    } else {
        printf("Unknown operation: %s\n", operation);
        exit(1);
    }
    
    if (send_command(sockfd, &server_addr, command, response) > 0) {
        printf("Response: %s\n", response);
    }
    
    close(sockfd);
    return 0;
}
