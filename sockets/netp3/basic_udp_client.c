/*
 * Basic UDP Socket Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * Basic UDP Socket - Client Side
 * 
 * Demonstrates how to send datagrams to a UDP server.
 * Client creates socket, sends message, receives response.
 * 
 * Key concepts:
 * 
 * 1. UDP client flow:
 *    ┌─────────────────────────────────────────────────────────┐
 *    │ 1. socket()                                             │
 *    │ 2. sendto() (specify server address)                   │
 *    │ 3. recvfrom() (receive response)                        │
 *    │ 4. close()                                              │
 *    └─────────────────────────────────────────────────────────┘
 * 
 * 2. Address specification:
 *    - Client must know server's address and port
 *    - No connection establishment needed
 *    - Each message is independent
 * 
 * Assumptions:
 *   - Connects to localhost port 8000
 *   - Sends message from command line argument
 *   - Receives and displays echo response
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1000
#define SERVER_PORT 8000

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in servaddr;
    socklen_t servlen;
    char buffer[BUFFER_SIZE];
    
    /*
     * Check command line arguments
     */
    if (argc != 2) {
        printf("Usage: %s <message>\n", argv[0]);
        printf("Example: %s 'Hello UDP Server'\n", argv[0]);
        return 1;
    }
    
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
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(SERVER_PORT);
    
    /*
     * Send message to server
     */
    servlen = sizeof(servaddr);
    printf("Sending message: %s\n", argv[1]);
    sendto(sockfd, argv[1], strlen(argv[1]), 0, 
           (struct sockaddr *)&servaddr, servlen);
    
    /*
     * Receive response from server
     */
    n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
    if (n < 0) {
        perror("recvfrom failed");
        close(sockfd);
        exit(1);
    }
    
    buffer[n] = '\0';
    printf("Server response: %s\n", buffer);
    
    /*
     * Cleanup
     */
    close(sockfd);
    return 0;
}
