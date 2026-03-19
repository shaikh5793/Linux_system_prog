/*
 * Basic UDP Socket Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * Basic UDP Socket - Server Side
 * 
 * This example demonstrates connectionless UDP communication.
 * Unlike TCP, UDP doesn't maintain connections - each message
 * is independent and requires explicit addressing.
 * 
 * Key concepts:
 * 
 * 1. UDP socket programming:
 *    - Connectionless: no explicit connection setup
 *    - Datagram-based: message boundaries preserved
 *    - Unreliable: no delivery guarantees (faster than TCP)
 * 
 * 2. Basic UDP server lifecycle:
 *    ┌─────────────────────────────────────────────────────────┐
 *    │ 1. socket()                                             │
 *    │ 2. bind()                                               │
 *    │ 3. recvfrom() → process → sendto() (echo)              │
 *    └─────────────────────────────────────────────────────────┘
 * 
 * 3. Key differences from TCP:
 *    - No listen() or accept() calls
 *    - recvfrom() and sendto() include address information
 *    - Each datagram is independent
 *    - Client address obtained from recvfrom()
 * 
 * Assumptions:
 *   - Runs on localhost port 8000
 *   - Echoes received messages back to sender
 *   - Handles one message at a time (blocking)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1000
#define SERVER_PORT 8000

int main() {
    int sockfd, retval, n;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char buf[BUFFER_SIZE];
    
    /*
     * Socket creation - UDP
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(1);
    }
    
    /*
     * Server address configuration
     */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);
    
    /*
     * Bind socket to address
     */
    retval = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (retval < 0) {
        perror("bind failed");
        exit(2);
    }
    
    printf("UDP server listening on port %d\n", SERVER_PORT);
    printf("Ready to receive datagrams...\n");
    
    /*
     * Main server loop
     */
    clilen = sizeof(struct sockaddr_in);
    while (1) {
        /*
         * Receive datagram from client
         * recvfrom() blocks until message arrives
         */
        n = recvfrom(sockfd, buf, BUFFER_SIZE - 1, 0, 
                     (struct sockaddr *)&cliaddr, &clilen);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        /*
         * Process received message
         */
        buf[n] = '\0';
        printf("Received from client: %s\n", buf);
        
        /*
         * Echo message back to client
         * sendto() sends to specific client address
         */
        sendto(sockfd, buf, n, 0, 
               (struct sockaddr *)&cliaddr, clilen);
        printf("Echo sent back to client\n");
    }
    
    close(sockfd);
    return 0;
}
