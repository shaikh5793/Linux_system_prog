/*
 * Basic TCP Socket Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * BASIC TCP SOCKET - SERVER SIDE
 * 
 * This example illustrates a simple TCP server using blocking sockets.
 * It accepts a single client connection at a time, echoes received messages,
 * and demonstrates fundamental socket programming concepts.
 * 
 * KEY CONCEPTS:
 * 
 * 1. TCP Socket Programming:
 *    - Reliable, connection-oriented communication
 *    - Stream-based: No inherent message boundaries (handled by the application)
 * 
 * 2. Basic Server Lifecycle:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ 1. socket()                                                         │
 *    │ 2. bind()                                                           │
 *    │ 3. listen()                                                         │
 *    │ 4. accept()        --->  Handle Connection (recv/send) ---> close() │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. Block Diagram:
 *    ┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐
 *    │                   │   │                   │   │                   │
 *    │ Server Socket     │   │ Client Socket     │   │ Handle Client     │
 *    │                   │<--│                   │   │  + Read/Write     │
 *    │  + socket()       │   │  + connect()      │<--│  + Echo Messages  │
 *    │  + bind()         │   │                   │   │                   │
 *    │  + listen()       │   │                   │   │                   │
 *    │                   │   │                   │   │                   │
 *    └───────────────────┘   └───────────────────┘   └───────────────────┘
 * 
 * 4. Key Functions:
 *    - socket(): Create socket descriptor
 *    - bind(): Associate socket with IP address and port
 *    - listen(): Prepare to accept incoming connections
 *    - accept(): Accept an incoming connection
 *    - read()/write(): Handle communication with client
 * 
 * ASSUMPTIONS:
 *   - The server handles only one client at a time (sequentially)
 *   - It runs on localhost, listening on port 8000
 *   - IPv4 is used for networking
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>

int main() {
    int listen_fd, conn_fd, retval;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char buffer[256];
    int n;

    /*
     * SOCKET CREATION
     * socket() creates a new socket descriptor
     * - AF_INET: IPv4
     * - SOCK_STREAM: TCP
     * - 0: Default protocol (TCP)
     */
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket:");
        exit(1);
    }

    /*
     * ADDRESS CONFIGURATION
     * Fill server address structure with desired parameters
     */
    /*
     * - sin_family: Address family (AF_INET for IPv4)
     * - sin_addr: IP address (htonl INADDR_ANY binds to all interfaces)
     * - sin_port: Port number (htons converts to network byte order)
     */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8000);

    /*
     * BIND OPERATION
     * assign address to socket
     */
    retval = bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (retval < 0) {
        perror("bind:");
        exit(2);
    }

    /*
     * LISTENING FOR CONNECTIONS
     * Marks the socket as a passive socket, awaiting connection requests
     */
    listen(listen_fd, 5);
    printf("TCP server listening on port 8000\n");

    /*
     * MAIN SERVER LOOP
     * Accepts and processes client requests
     */
    while (1) {
        /*
         * ACCEPT CONNECTION
         * Blocks waiting for incoming connection
         */
        clilen = sizeof(cliaddr);
        printf("Waiting for connection...\n");
        conn_fd = accept(listen_fd, (struct sockaddr*)&cliaddr, &clilen);

        /*
         * CONNECTION ESTABLISHED
         * Client is connected; we can communicate
         */
        printf("Client connected\n");
        printf("Client's port no = %d\n", ntohs(cliaddr.sin_port));

        /*
         * COMMUNICATE WITH CLIENT
         * Echo received messages back to client
         */
        while (1) {
            n = read(conn_fd, buffer, sizeof(buffer) - 1);
            if (n <= 0) {
                printf("Client disconnected\n");
                break;
            }
            buffer[n] = '\0';
            printf("Received: %s\n", buffer);
            write(conn_fd, buffer, n);
        }

        /*
         * CLOSE CONNECTION
         * Disconnect from client after communication ends
         */
        close(conn_fd);
    }

    return 0;
}
