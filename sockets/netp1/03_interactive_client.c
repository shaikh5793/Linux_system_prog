/*
 * Interactive Unix Domain Socket Client (SOCK_STREAM)
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * INTERACTIVE UNIX DOMAIN SOCKET - STREAM CLIENT
 * 
 * This example demonstrates persistent connections where a client
 * can send multiple messages in a single session.
 * 
 * HOW THIS DIFFERS FROM BASIC STREAM (01_stream_*):
 * 
 * BASIC STREAM CLIENT:        INTERACTIVE STREAM CLIENT:
 * socket()                    socket()
 * connect()                   connect()
 * send(one_message)           while(user_input) {
 * recv(one_response)            send(message)
 * close()                       recv(response)
 *                             }
 *                             close()
 * 
 * KEY ADVANTAGES OF INTERACTIVE PATTERN:
 * 
 * 1. Persistent Session (vs. Basic's Single Transaction):
 *    - Single connection for multiple message exchanges
 *    - Eliminates connection overhead for subsequent messages
 *    - Maintains server-side state between messages
 *    - Enables complex multi-step protocols
 * 
 * 2. Interactive Communication (vs. Basic's One-Shot):
 *    - User can type multiple messages without reconnecting
 *    - Each message gets individual response with session context
 *    - Special commands: 'quit', 'status' demonstrate protocol design
 *    - Real-time conversation pattern
 * 
 * 3. Session Lifecycle Management:
 *    connect() → send/recv loop → graceful close()
 *    - Proper session initialization and cleanup
 *    - Error handling during multi-message session
 *    - Graceful vs. abrupt disconnection handling
 * 
 * 4. Real-World Applications:
 *    - Database connections (connect once, multiple queries)
 *    - Chat clients (persistent conversation)
 *    - Shell protocols (command session)
 *    - FTP-like control connections
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/interactive_stream_socket"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    const char *initial_message = NULL;
    ssize_t bytes_sent, bytes_received;
    int message_count = 0;
    
    if (argc == 2) {
        initial_message = argv[1];
    }
    
    printf("=== Interactive Unix Domain Socket Client ===\n");
    printf("Target server: %s\n", SOCKET_PATH);
    if (initial_message) {
        printf("Initial message: %s\n", initial_message);
    }
    printf("\n");
    
    /*
     * Create Unix domain socket for interactive session
     */
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Interactive socket created (fd=%d)\n", client_fd);
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    /*
     * Establish persistent connection to server
     */
    printf("Connecting to interactive server...\n");
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: connect() failed");
        printf("\nTroubleshooting:\n");
        printf("- Is the interactive server running?\n");
        printf("- Check if socket file exists: ls -la %s\n", SOCKET_PATH);
        close(client_fd);
        exit(1);
    }
    printf("✓ Connected to interactive server\n\n");
    
    /*
     * Send initial message if provided
     */
    if (initial_message) {
        message_count++;
        printf("=== Message #%d ===\n", message_count);
        printf("Sending: %s\n", initial_message);
        
        bytes_sent = send(client_fd, initial_message, strlen(initial_message), 0);
        if (bytes_sent == -1) {
            perror("ERROR: send() failed");
            close(client_fd);
            exit(1);
        }
        printf("✓ Sent %zd bytes\n", bytes_sent);
        
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("✓ Response (%zd bytes): %s\n", bytes_received, buffer);
        }
        printf("\n");
    }
    
    /*
     * CRITICAL DIFFERENCE: Interactive session loop
     * 
     * Unlike basic stream (01_*) which does:
     *   connect() → send() → recv() → close()
     * 
     * Interactive stream maintains persistent connection:
     *   connect() → while(user_input) { send() → recv() } → close()
     * 
     * Benefits:
     * - No reconnection overhead for multiple messages
     * - Server maintains session state between messages
     * - Enables complex multi-step protocols
     */
    printf("=== Interactive Session Started ===\n");
    printf("Commands:\n");
    printf("  - Type any message to send to server\n");
    printf("  - 'status' - Get session status\n");
    printf("  - 'quit' - End session gracefully\n");
    printf("  - Ctrl+C - Force disconnect\n");
    printf("\n");
    
    while (1) {
        printf("Message: ");
        fflush(stdout);
        
        /*
         * Read user input
         */
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nEOF detected, ending session\n");
            break;
        }
        
        // Remove newline character
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // Skip empty messages
        if (strlen(buffer) == 0) {
            continue;
        }
        
        message_count++;
        
        /*
         * Send message to server
         * Connection remains open for multiple sends
         */
        bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
        if (bytes_sent == -1) {
            perror("ERROR: send() failed");
            break;
        }
        printf("Sent %zd bytes\n", bytes_sent);
        
        /*
         * Receive response from server
         */
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("ERROR: recv() failed");
            break;
        } else if (bytes_received == 0) {
            printf("Server closed connection\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Response: %s\n", buffer);
        
        /*
         * Check if server confirmed quit command
         */
        if (strstr(buffer, "ending") != NULL || strstr(buffer, "Goodbye") != NULL) {
            printf("Session terminated by server\n");
            break;
        }
        
        printf("\n");
    }
    
    /*
     * Clean up and close connection
     */
    close(client_fd);
    printf("\n✓ Interactive session ended\n");
    printf("Total messages sent: %d\n", message_count);
    
    return 0;
}
