/*
 * Basic TCP Socket Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * BASIC TCP SOCKET - CLIENT SIDE
 * 
 * This example demonstrates a simple TCP client that connects to a server,
 * sends a message, receives a response, and then disconnects.
 * 
 * KEY CONCEPTS:
 * 
 * 1. TCP Client Programming:
 *    - Reliable, connection-oriented communication
 *    - Stream-based: No inherent message boundaries
 *    - Client initiates connection to server
 * 
 * 2. Basic Client Lifecycle:
 *    ┌─────────────────────────────────────────────────────────────┐
 *    │ 1. socket()                                                 │
 *    │ 2. connect()                                                │
 *    │ 3. write()/read() ──► Exchange Data with Server             │
 *    │ 4. close()                                                  │
 *    └─────────────────────────────────────────────────────────────┘
 * 
 * 3. Communication Flow:
 *    Client                    Server
 *      │                        │
 *      ├──── connect() ────────►│
 *      │    ◄─── accept() ──────┤
 *      ├──── write(msg) ───────►│
 *      │    ◄─── read(msg) ─────┤
 *      │    ◄─── write(echo) ───┤
 *      ├──── read(echo) ───────►│
 *      ├──── close() ──────────►│
 *      │                        │
 * 
 * 4. Key Functions:
 *    - socket(): Create socket descriptor
 *    - connect(): Establish connection to server
 *    - write()/read(): Send/receive data
 *    - close(): Terminate connection
 * 
 * ASSUMPTIONS:
 *   - Connects to server running on localhost:8000
 *   - Sends one message and expects one response
 *   - IPv4 is used for networking
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    /*
     * VARIABLE DECLARATIONS
     */
    char *serv_ip = "127.0.0.1";  // Server IP address (localhost)
    int sockfd, ret_val, n;        // Socket descriptor and return values
    struct sockaddr_in servaddr;   // Server address structure
    char buffer[256];              // Buffer for received data

    /*
     * COMMAND LINE ARGUMENT VALIDATION
     */
    if (argc != 2) {
        printf("Usage: %s <message>\n", argv[0]);
        printf("Example: %s \"Hello Server\"\n", argv[0]);
        return 1;
    }

    /*
     * STEP 1: CREATE SOCKET
     * socket() creates a new socket descriptor
     * - AF_INET: IPv4
     * - SOCK_STREAM: TCP (reliable, connection-oriented)
     * - 0: Default protocol (TCP)
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Socket created (fd=%d)\n", sockfd);

    /*
     * STEP 2: CONFIGURE SERVER ADDRESS
     * Fill server address structure with connection details
     */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;              // IPv4
    servaddr.sin_port = htons(8000);            // Port 8000 (network byte order)
    inet_pton(AF_INET, serv_ip, &servaddr.sin_addr);  // Convert IP string to binary
    
    printf("✓ Server address configured: %s:8000\n", serv_ip);

    /*
     * STEP 3: ESTABLISH CONNECTION
     * connect() initiates a connection to the server
     * This performs the TCP 3-way handshake
     */
    printf("Attempting to connect to server...\n");
    ret_val = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret_val < 0) {
        perror("ERROR: connect() failed");
        close(sockfd);
        exit(1);
    }
    printf("✓ Connection established with server\n");

    /*
     * STEP 4: SEND MESSAGE TO SERVER
     * write() sends data through the connected socket
     */
    printf("Sending message: \"%s\"\n", argv[1]);
    ssize_t bytes_sent = write(sockfd, argv[1], strlen(argv[1]));
    if (bytes_sent < 0) {
        perror("ERROR: write() failed");
        close(sockfd);
        exit(1);
    }
    printf("✓ Message sent (%zd bytes)\n", bytes_sent);

    /*
     * STEP 5: RECEIVE RESPONSE FROM SERVER
     * read() receives data from the connected socket
     */
    printf("Waiting for server response...\n");
    n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        perror("ERROR: read() failed");
        close(sockfd);
        exit(1);
    }
    
    if (n == 0) {
        printf("Server closed connection\n");
    } else {
        buffer[n] = '\0';  // Null-terminate received data
        printf("✓ Received response (%d bytes): \"%s\"\n", n, buffer);
    }

    /*
     * STEP 6: CLOSE CONNECTION
     * close() terminates the connection and releases resources
     */
    close(sockfd);
    printf("✓ Connection closed\n");
    
    printf("\nTCP Client Session Complete\n");
    return 0;
}
