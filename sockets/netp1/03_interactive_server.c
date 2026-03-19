/*
 * Interactive Unix Domain Socket Server (SOCK_STREAM)
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * INTERACTIVE UNIX DOMAIN SOCKET - STREAM SERVER
 * 
 * This example demonstrates persistent connections and multiple message
 * exchanges within a single session using SOCK_STREAM.
 * 
 * HOW THIS DIFFERS FROM BASIC STREAM (01_stream_*):
 * 
 * BASIC STREAM:               INTERACTIVE STREAM:
 * connect()                   connect()
 * send(message)               send(message1)
 * recv(response)              recv(response1)
 * close()                     send(message2)
 *                             recv(response2)
 *                             send(message3)
 *                             recv(response3)
 *                             ...continue...
 *                             close()
 * 
 * KEY CONCEPTS:
 * 
 * 1. Persistent Connections (vs. Basic's One-Shot):
 *    - Connection remains open for multiple messages
 *    - Eliminates connection setup/teardown overhead
 *    - Enables stateful communication protocols
 *    - Server maintains session context between messages
 * 
 * 2. Session Management (vs. Basic's Stateless):
 *    - Each client gets a dedicated session with tracking
 *    - Server maintains per-session state (message count, client ID)
 *    - Proper cleanup when client disconnects
 *    - Session-aware error handling
 * 
 * 3. Interactive Communication Pattern:
 *    Client ──connect──► Server
 *      │                   │
 *      ├──message 1──────► │ (session state: msg#1)
 *      │ ◄────response──── │
 *      ├──message 2──────► │ (session state: msg#2)
 *      │ ◄────response──── │
 *      │        ...        │
 *      └──disconnect─────► │ (cleanup session)
 * 
 * 4. Protocol Commands (vs. Basic's Simple Echo):
 *    - Special commands: 'quit', 'status'
 *    - Context-aware responses with session info
 *    - Graceful session termination handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/interactive_stream_socket"
#define BUFFER_SIZE 1024

static int server_fd = -1;

void cleanup_and_exit(int sig) {
    (void)sig;
    printf("\n=== Interactive Server Shutdown ===\n");
    if (server_fd != -1) {
        close(server_fd);
        printf("Server socket closed\n");
    }
    if (unlink(SOCKET_PATH) == 0) {
        printf("Socket file removed: %s\n", SOCKET_PATH);
    }
    printf("Interactive server shutdown complete\n");
    exit(0);
}

int main() {
    struct sockaddr_un server_addr;
    int client_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int client_count = 0;
    
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    
    printf("=== Interactive Unix Domain Socket Server ===\n");
    printf("Socket type: SOCK_STREAM (persistent connections)\n");
    printf("Mode: Interactive (multiple messages per session)\n\n");
    
    /*
     * Create Unix domain socket for interactive communication
     */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Interactive socket created (fd=%d)\n", server_fd);
    
    unlink(SOCKET_PATH);
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: bind() failed");
        close(server_fd);
        exit(1);
    }
    printf("✓ Socket bound to path: %s\n", SOCKET_PATH);
    
    if (listen(server_fd, 5) == -1) {
        perror("ERROR: listen() failed");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(1);
    }
    printf("✓ Server listening for interactive sessions\n");
    printf("\nServer ready! Press Ctrl+C to stop\n");
    printf("Waiting for client connections...\n\n");
    
    /*
     * Accept and handle interactive client sessions
     * Each client can send multiple messages before disconnecting
     */
    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) continue;
            perror("ERROR: accept() failed");
            continue;
        }
        
        client_count++;
        printf("=== Interactive Session #%d Started ===\n", client_count);
        printf("Client fd: %d\n", client_fd);
        
        int message_count = 0;
        
        /*
         * CRITICAL DIFFERENCE: Interactive message loop for this client
         * 
         * Unlike basic stream (01_*) which does:
         *   accept() → recv() → send() → close()
         * 
         * Interactive stream does:
         *   accept() → while(connected) { recv() → send() } → close()
         * 
         * This enables:
         * - Multiple messages per connection
         * - Session state maintenance
         * - Protocol-aware command handling
         */
        while (1) {
            /*
             * Receive message from client
             * recv() will return 0 when client disconnects gracefully
             */
            bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytes_received == -1) {
                perror("ERROR: recv() failed");
                break;
            } else if (bytes_received == 0) {
                printf("Client disconnected gracefully\n");
                break;
            }
            
            message_count++;
            buffer[bytes_received] = '\0';
            
            printf("Message #%d (%zd bytes): %s\n", message_count, bytes_received, buffer);
            
            /*
             * Process the message and send response
             * Each message gets a unique response with session info
             */
                char response[BUFFER_SIZE + 100];  // Increased buffer size for safety
            
            // Special handling for certain commands
            if (strcmp(buffer, "quit") == 0) {
                snprintf(response, sizeof(response), 
                        "Session #%d ending. Processed %d messages. Goodbye!", 
                        client_count, message_count);
                send(client_fd, response, strlen(response), 0);
                printf("Client requested session termination\n");
                break;
            } else if (strcmp(buffer, "status") == 0) {
                snprintf(response, sizeof(response), 
                        "Session #%d active. Message count: %d", 
                        client_count, message_count);
            } else {
                snprintf(response, sizeof(response), 
                        "Session #%d Echo: %s [Msg #%d, %zd bytes]", 
                        client_count, buffer, message_count, bytes_received);
            }
            
            ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
            if (bytes_sent == -1) {
                perror("ERROR: send() failed");
                break;
            } else {
                printf("Response sent (%zd bytes)\n", bytes_sent);
            }
            
            printf("Waiting for next message from client...\n");
        }
        
        /*
         * Session cleanup
         * Close client connection and prepare for next client
         */
        close(client_fd);
        printf("=== Session #%d Ended ===\n", client_count);
        printf("Total messages in session: %d\n", message_count);
        printf("Waiting for next client connection...\n\n");
    }
    
    cleanup_and_exit(0);
    return 0;
}
