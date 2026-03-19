/*
 * Connected UDP Socket Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * Connected UDP Socket - Client Side
 * 
 * This example demonstrates a UDP client that uses connect() to establish
 * a "connection" to a specific server, allowing the use of send()/recv()
 * instead of sendto()/recvfrom().
 * 
 * Key concepts:
 * 
 * 1. Connected UDP vs Regular UDP:
 *    ┌─────────────────────────────────────────────────────────────┐
 *    │                    Regular UDP Client                       │
 *    ├─────────────────────────────────────────────────────────────┤
 *    │ 1. socket(AF_INET, SOCK_DGRAM, 0)                          │
 *    │ 2. [optional: bind() for specific local address]           │
 *    │ 3. sendto(sockfd, data, len, 0, server_addr, addr_len)     │
 *    │ 4. recvfrom(sockfd, buf, len, 0, server_addr, &addr_len)   │
 *    │ 5. close(sockfd)                                           │
 *    └─────────────────────────────────────────────────────────────┘
 *                              vs
 *    ┌─────────────────────────────────────────────────────────────┐
 *    │                   Connected UDP Client                      │
 *    ├─────────────────────────────────────────────────────────────┤
 *    │ 1. socket(AF_INET, SOCK_DGRAM, 0)                          │
 *    │ 2. [optional: bind() for specific local address]           │
 *    │ 3. connect(sockfd, server_addr, addr_len)  // "Connect"    │
 *    │ 4. send(sockfd, data, len, 0)              // Simpler!     │
 *    │ 5. recv(sockfd, buf, len, 0)               // Simpler!     │
 *    │ 6. close(sockfd)                                           │
 *    └─────────────────────────────────────────────────────────────┘
 * 
 * 2. What connect() does for UDP:
 *    - Stores the server address in the socket structure
 *    - Enables use of send()/recv() instead of sendto()/recvfrom()
 *    - Filters incoming packets (only from connected peer)
 *    - Enables better error reporting (ICMP errors delivered)
 *    - NO actual network connection is established (still UDP!)
 * 
 * 3. Benefits of connected UDP:
 *    - Simplified API: No need to specify destination repeatedly
 *    - Better error detection: ICMP errors (port unreachable, etc.)
 *    - Security: Prevents spoofed packets from other sources
 *    - Performance: Kernel optimization (cached destination)
 *    - Convenience: More similar to TCP programming model
 * 
 * 4. Limitations of connected UDP:
 *    - Can only communicate with ONE peer at a time
 *    - Still unreliable (no delivery guarantees)
 *    - No flow control or congestion control
 *    - Must "disconnect" to talk to different peer
 *    - Not suitable for broadcast/multicast
 * 
 * 5. Error Reporting Enhancement:
 *    With connected UDP, ICMP errors are delivered to the socket:
 *    - ECONNREFUSED: Port unreachable
 *    - EHOSTUNREACH: Host unreachable
 *    - ENETUNREACH: Network unreachable
 *    
 *    Regular UDP clients typically don't receive these errors.
 * 
 * 
 * Assumptions:
 *   - Connects to localhost port 8082
 *   - Demonstrates connected UDP features
 *   - Handles enhanced error reporting
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>  // socket(), connect(), send(), recv()
#include <sys/un.h>      // Not used here, but included for consistency
#include <netinet/in.h>  // struct sockaddr_in, htons()
#include <arpa/inet.h>   // inet_addr()
#include <errno.h>
#include <signal.h>

#define SERVER_PORT 8082
#define BUFFER_SIZE 1024

static int client_fd = -1;

void cleanup_and_exit(int sig) {
    (void)sig;
    printf("\n=== Connected UDP Client Shutdown ===\n");
    if (client_fd != -1) {
        close(client_fd);
        printf("Client socket closed\n");
    }
    printf("Connected UDP client shutdown complete\n");
    exit(0);
}

int main() {
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    ssize_t bytes_sent, bytes_received;
    
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    
    printf("=== Connected UDP Socket Client ===\n");
    printf("Socket type: SOCK_DGRAM (UDP) with connect()\n");
    printf("Server: 127.0.0.1:%d\n", SERVER_PORT);
    printf("Mode: Connected (using send/recv instead of sendto/recvfrom)\n\n");
    
    /*
     * STEP 1: Create UDP socket
     * 
     * socket(AF_INET, SOCK_DGRAM, 0)
     * - AF_INET: Internet domain socket (IPv4)
     * - SOCK_DGRAM: UDP datagram socket (unreliable, message-oriented)
     * - 0: Protocol (automatic selection for UDP)
     * 
     * NOTE: Socket creation is identical for connected and regular UDP
     */
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ UDP socket created (fd=%d)\n", client_fd);
    
    /*
     * STEP 2: Setup server address structure
     * 
     * This defines WHERE we want to "connect" to
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost
    server_addr.sin_port = htons(SERVER_PORT);
    
    printf("✓ Server address configured: 127.0.0.1:%d\n", SERVER_PORT);
    
    /*
     * STEP 3: Connect to server (UDP "connection")
     * 
     * connect() for UDP sockets:
     * - Does NOT send any packets to the server
     * - Does NOT establish a real connection (UDP is connectionless)
     * - Simply stores the server address in the socket
     * - Enables use of send()/recv() instead of sendto()/recvfrom()
     * - Filters incoming packets (only from this peer)
     * - Enables better error reporting
     * 
     * CRITICAL DIFFERENCE from TCP:
     * - TCP connect() performs 3-way handshake
     * - UDP connect() is purely local operation (no network traffic)
     * - UDP connect() cannot fail due to server being down
     */
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: connect() failed");
        close(client_fd);
        exit(1);
    }
    printf("✓ UDP socket \"connected\" to server\n");
    printf("  Note: This is a local operation, no packets sent yet!\n");
    printf("  Now we can use send()/recv() instead of sendto()/recvfrom()\n\n");
    
    /*
     * STEP 4: Interactive communication loop
     * 
     * Now we can use send() and recv() just like TCP, but:
     * - Packets can still be lost (UDP is unreliable)
     * - No delivery guarantees
     * - No flow control
     * - Better error reporting than regular UDP
     */
    printf("Connected UDP Client Features:\n");
    printf("• Using send()/recv() instead of sendto()/recvfrom()\n");
    printf("• Better error detection (ICMP errors delivered)\n");
    printf("• Packets filtered by source (only from connected server)\n");
    printf("• Still unreliable (UDP characteristics preserved)\n\n");
    
    printf("Special commands: STATS, HELP, QUIT\n");
    printf("Enter messages (empty line to quit):\n\n");
    
    while (1) {
        printf("Message> ");
        fflush(stdout);
        
        // Read message from user
        if (fgets(message, sizeof(message), stdin) == NULL) {
            printf("\nEnd of input received\n");
            break;
        }
        
        // Remove trailing newline
        size_t len = strlen(message);
        if (len > 0 && message[len-1] == '\n') {
            message[len-1] = '\0';
            len--;
        }
        
        // Exit on empty message
        if (len == 0) {
            printf("Empty message, exiting...\n");
            break;
        }
        
        /*
         * Send message using send() instead of sendto()
         * 
         * send() for connected UDP:
         * - No need to specify destination address
         * - Destination was set during connect()
         * - Much simpler than sendto()
         * - Returns same values as sendto()
         * 
         * Benefits:
         * - Cleaner code (no address management)
         * - Less prone to errors (no wrong destination)
         * - Slightly more efficient (address cached)
         */
        printf("Sending message via connected UDP...\n");
        bytes_sent = send(client_fd, message, strlen(message), 0);
        
        if (bytes_sent == -1) {
            /*
             * Enhanced error reporting with connected UDP!
             * 
             * With regular UDP, these errors often go unnoticed.
             * With connected UDP, ICMP errors are delivered:
             */
            if (errno == ECONNREFUSED) {
                printf("ERROR: Server port is not listening (connection refused)\n");
            } else if (errno == EHOSTUNREACH) {
                printf("ERROR: Server host is unreachable\n");
            } else if (errno == ENETUNREACH) {
                printf("ERROR: Network is unreachable\n");
            } else {
                perror("ERROR: send() failed");
            }
            continue;
        }
        
        printf("✓ Message sent (%zd bytes)\n", bytes_sent);
        
        /*
         * Receive response using recv() instead of recvfrom()
         * 
         * recv() for connected UDP:
         * - No need for source address parameter
         * - Only receives packets from connected peer
         * - Packets from other sources are filtered out
         * - Returns same values as recvfrom()
         * 
         * Security benefit:
         * - Cannot receive spoofed packets from other sources
         * - Only the "connected" peer can send us packets
         */
        printf("Waiting for response from connected server...\n");
        bytes_received = recv(client_fd, response, sizeof(response) - 1, 0);
        
        if (bytes_received == -1) {
            if (errno == ECONNREFUSED) {
                printf("ERROR: Server responded with port unreachable\n");
            } else {
                perror("ERROR: recv() failed");
            }
            continue;
        }
        
        if (bytes_received == 0) {
            printf("Received empty response\n");
            continue;
        }
        
        // Null-terminate and display response
        response[bytes_received] = '\0';
        
        printf("=== Response Received ===\n");
        printf("Size: %zd bytes\n", bytes_received);
        printf("Data: '%s'\n", response);
        printf("=========================\n\n");
        
        // Check for quit confirmation
        if (strstr(response, "Goodbye") != NULL) {
            printf("Server confirmed quit, disconnecting...\n");
            break;
        }
    }
    
    /*
     * STEP 5: Demonstrate disconnection (optional)
     * 
     * To "disconnect" a UDP socket and make it regular again:
     * - connect() with NULL address, or
     * - connect() with AF_UNSPEC family
     * 
     * After disconnection, must use sendto()/recvfrom() again
     */
    printf("\nDemonstrating UDP disconnection...\n");
    
    struct sockaddr disconnect_addr;
    memset(&disconnect_addr, 0, sizeof(disconnect_addr));
    disconnect_addr.sa_family = AF_UNSPEC;  // Unspecified family = disconnect
    
    if (connect(client_fd, &disconnect_addr, sizeof(disconnect_addr)) == -1) {
        // Disconnection can fail on some systems, but that's okay
        printf("Note: Disconnection failed (not critical)\n");
    } else {
        printf("✓ UDP socket disconnected\n");
        printf("  Socket is now in regular UDP mode\n");
        printf("  Would need to use sendto()/recvfrom() for further communication\n");
    }
    
    printf("\nConnected UDP Client Features Demonstrated:\n");
    printf("- connect() for UDP (local operation, no network traffic)\n");
    printf("- send()/recv() API instead of sendto()/recvfrom()\n");
    printf("- Enhanced error reporting (ICMP errors delivered)\n");
    printf("- Source filtering (only connected peer can send packets)\n");
    printf("- UDP disconnection (back to regular UDP mode)\n");
    printf("- Simplified programming model (similar to TCP)\n");
    
    printf("\nKey Differences from TCP:\n");
    printf("- Still unreliable (no delivery guarantees)\n");
    printf("- No flow control or congestion control\n");
    printf("- No connection establishment handshake\n");
    printf("- connect() is purely local (no server involvement)\n");
    printf("- Can be disconnected and reconnected to different peers\n");
    
    cleanup_and_exit(0);
    return 0;
}
