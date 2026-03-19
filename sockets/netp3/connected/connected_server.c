/*
 * UDP Connected Socket Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UDP connected socket - server side
 * 
 * This example demonstrates a UDP server that can work with both
 * regular UDP clients and connected UDP clients.
 * 
 * Key concepts:
 * 
 * 1. What is a connected UDP socket?
 *    - UDP socket that has called connect() to associate with a specific peer
 *    - Still uses UDP protocol (unreliable, connectionless)
 *    - No actual "connection" is established (unlike TCP)
 *    - Simply stores the destination address in the socket
 * 
 * 2. Server perspective:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ Server code remains unchanged for both client types              │
 *    │ Server uses recvfrom() to receive from any client               │
 *    │ Server cannot tell if client is "connected" or not              │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. Connected UDP advantages:
 *    - Simplified API: send()/recv() instead of sendto()/recvfrom()
 *    - Better error reporting: ICMP errors delivered to connected socket
 *    - Security: Prevents receiving packets from other sources
 *    - Performance: Kernel caches destination address
 * 
 * 4. Connected UDP limitations:
 *    - Still unreliable (packets can be lost, duplicated, reordered)
 *    - No flow control or congestion control
 *    - No handshake or connection establishment
 *    - Can only communicate with one peer at a time
 * 
 * Assumptions:
 *   - Server listens on port 8082
 *   - Works with both connected and regular UDP clients
 *   - Tracks client statistics for demonstration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>  // socket(), bind(), recvfrom(), sendto()
#include <sys/un.h>      // Not used here, but included for consistency
#include <netinet/in.h>  // struct sockaddr_in, htons(), htonl()
#include <arpa/inet.h>   // inet_addr(), inet_ntoa()
#include <errno.h>
#include <signal.h>
#include <time.h>

#define SOCKET_PORT 8082
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

/*
 * Client tracking structure for demonstration purposes
 * Helps show the difference between connected and regular UDP clients
 */
struct client_info {
    struct sockaddr_in addr;    // Client's address
    int message_count;          // Number of messages received
    time_t first_seen;          // When client first contacted us
    time_t last_seen;           // Last message timestamp
    int active;                 // Whether client is active
    char client_type[32];       // For demo: "connected" or "regular"
};

static int server_fd = -1;
static struct client_info clients[MAX_CLIENTS];
static int active_clients = 0;

void cleanup_and_exit(int sig) {
    (void)sig;
    printf("\n=== Connected UDP Server Shutdown ===\n");
    if (server_fd != -1) {
        close(server_fd);
        printf("Server socket closed\n");
    }
    printf("Connected UDP server shutdown complete\n");
    exit(0);
}

/*
 * Find existing client by address
 * Returns client index or -1 if not found
 */
int find_client(struct sockaddr_in *client_addr) {
    for (int i = 0; i < active_clients; i++) {
        if (clients[i].active &&
            clients[i].addr.sin_addr.s_addr == client_addr->sin_addr.s_addr &&
            clients[i].addr.sin_port == client_addr->sin_port) {
            return i;
        }
    }
    return -1;
}

/*
 * Add new client to tracking list
 * Returns client index or -1 if list is full
 */
int add_client(struct sockaddr_in *client_addr) {
    if (active_clients >= MAX_CLIENTS) {
        printf("WARNING: Maximum clients reached, cannot track new client\n");
        return -1;
    }
    
    int index = active_clients;
    clients[index].addr = *client_addr;
    clients[index].message_count = 1;
    clients[index].first_seen = time(NULL);
    clients[index].last_seen = time(NULL);
    clients[index].active = 1;
    strcpy(clients[index].client_type, "unknown");  // Will be determined by behavior
    
    active_clients++;
    
    printf("New client tracked: %s:%d (index %d)\n",
           inet_ntoa(client_addr->sin_addr), 
           ntohs(client_addr->sin_port),
           index);
    
    return index;
}

/*
 * Update client statistics
 */
void update_client_stats(int client_index) {
    if (client_index >= 0 && client_index < active_clients) {
        clients[client_index].message_count++;
        clients[client_index].last_seen = time(NULL);
    }
}

/*
 * Display client statistics 
 */
void show_client_stats() {
    printf("\n=== Client Statistics ===\n");
    time_t now = time(NULL);
    
    for (int i = 0; i < active_clients; i++) {
        if (clients[i].active) {
            printf("Client %d: %s:%d\n", i,
                   inet_ntoa(clients[i].addr.sin_addr),
                   ntohs(clients[i].addr.sin_port));
            printf("  Messages received: %d\n", clients[i].message_count);
            printf("  Session duration: %ld seconds\n", now - clients[i].first_seen);
            printf("  Last activity: %ld seconds ago\n", now - clients[i].last_seen);
            printf("  Estimated type: %s\n", clients[i].client_type);
            printf("\n");
        }
    }
    printf("=========================\n\n");
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received, bytes_sent;
    
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    
    printf("=== Connected UDP Socket Server ===\n");
    printf("Socket type: SOCK_DGRAM (UDP, connectionless)\n");
    printf("Compatibility: Works with both connected and regular UDP clients\n");
    printf("Port: %d\n\n", SOCKET_PORT);
    
    /*
     * Step 1: Create UDP socket
     * 
     * socket(AF_INET, SOCK_DGRAM, 0)
     * - AF_INET: Internet domain socket (IPv4)
     * - SOCK_DGRAM: UDP datagram socket (unreliable, message-oriented)
     * - 0: Protocol (automatic selection for UDP)
     * 
     * Note: This is identical for both connected and regular UDP
     */
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ UDP socket created (fd=%d)\n", server_fd);
    
    /*
     * Step 2: Setup server address structure
     * 
     * Server address is the same regardless of client type
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost
    server_addr.sin_port = htons(SOCKET_PORT);
    
    /*
     * Enable address reuse (helpful during development)
     */
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("WARNING: setsockopt SO_REUSEADDR failed");
        // Continue anyway - not critical
    }
    
    /*
     * Step 3: Bind socket to address
     * 
     * This associates the socket with a specific IP address and port
     */
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: bind() failed");
        close(server_fd);
        exit(1);
    }
    printf("✓ Socket bound to 127.0.0.1:%d\n", SOCKET_PORT);
    
    printf("\nServer ready! Supports both client types:\n");
    printf("• Connected UDP clients (using connect/send/recv)\n");
    printf("• Regular UDP clients (using sendto/recvfrom)\n");
    printf("Press Ctrl+C to stop\n\n");
    
    printf("Special commands:\n");
    printf("• Send 'STATS' to see client statistics\n");
    printf("• Send 'HELP' for connection information\n");
    printf("• Send 'QUIT' for graceful disconnection\n\n");
    
    /*
     * Step 4: Main server loop
     * 
     * Critical point: Server code is identical regardless of client type!
     * - We always use recvfrom() to receive (gets sender address)
     * - We always use sendto() to reply (specifies destination)
     * - Connected UDP clients appear identical to regular UDP clients
     */
    while (1) {
        printf("Waiting for UDP message from any client...\n");
        
        /*
         * Receive message from any client
         * 
         * recvfrom() usage for UDP server:
         * - Works with both connected and regular UDP clients
         * - Always provides sender's address information
         * - Essential for UDP servers to know where to reply
         * 
         * Important: Even if client used send() (connected UDP),
         * Server receives it normally with recvfrom()
         */
        bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes_received == -1) {
            if (errno == EINTR) {
                continue;  // Interrupted by signal
            }
            perror("ERROR: recvfrom() failed");
            continue;
        }
        
        if (bytes_received == 0) {
            printf("Received empty datagram\n");
            continue;
        }
        
        // Null-terminate received data
        buffer[bytes_received] = '\0';
        
        printf("=== Message Received ===\n");
        printf("From: %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));
        printf("Size: %zd bytes\n", bytes_received);
        printf("Data: '%s'\n", buffer);
        
        /*
         * Client tracking for educational demonstration
         * In production, this might not be necessary
         */
        int client_index = find_client(&client_addr);
        if (client_index == -1) {
            client_index = add_client(&client_addr);
        } else {
            update_client_stats(client_index);
        }
        
        /*
         * Process message and generate response
         * Handle special commands for demonstration
         */
        char response[BUFFER_SIZE];
        
        if (strcmp(buffer, "STATS") == 0) {
            show_client_stats();
            snprintf(response, sizeof(response), 
                    "Client statistics displayed on server console. Active clients: %d", 
                    active_clients);
        }
        else if (strcmp(buffer, "HELP") == 0) {
            snprintf(response, sizeof(response),
                    "Connected UDP Server Help:\n"
                    "- This server works with both connected and regular UDP clients\n"
                    "- Connected clients can use send()/recv() after connect()\n"
                    "- Regular clients use sendto()/recvfrom()\n"
                    "- Both types appear identical to the server");
        }
        else if (strcmp(buffer, "QUIT") == 0) {
            snprintf(response, sizeof(response), 
                    "Goodbye! Session stats: %d messages received", 
                    client_index >= 0 ? clients[client_index].message_count : 0);
            
            // Mark client as inactive
            if (client_index >= 0) {
                clients[client_index].active = 0;
                printf("Client marked as disconnected\n");
            }
        }
        else {
            // Regular echo response with server information
            snprintf(response, sizeof(response),
                    "Connected UDP Server Echo: '%.950s' [Client %d, Message #%d]",
                    buffer,
                    client_index >= 0 ? client_index : -1,
                    client_index >= 0 ? clients[client_index].message_count : 1);
        }
        
        /*
         * Send response back to client
         * 
         * sendto() usage for UDP server:
         * - Must specify destination address (from recvfrom)
         * - Works regardless of how client sent the message
         * - Client will receive via recv() if connected, recvfrom() if not
         * 
         * Critical: We always use sendto() because:
         * 1. Server socket is not "connected" to any specific client
         * 2. Server needs to reply to the specific client that sent message
         * 3. UDP servers typically handle multiple clients
         */
        bytes_sent = sendto(server_fd, response, strlen(response), 0,
                          (struct sockaddr*)&client_addr, client_len);
        
        if (bytes_sent == -1) {
            perror("ERROR: sendto() failed");
        } else {
            printf("Response sent (%zd bytes): '%s'\n", bytes_sent, response);
        }
        
        printf("=== Message Processed ===\n\n");
    }
    
    // This point is never reached in normal operation
    cleanup_and_exit(0);
    return 0;
}
