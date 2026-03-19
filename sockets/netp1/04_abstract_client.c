/*
 * Unix Domain Socket Abstract Namespace Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UNIX DOMAIN SOCKET - ABSTRACT NAMESPACE CLIENT
 * 
 * This example demonstrates how a client connects to a server using
 * the abstract namespace for Unix domain sockets.
 * 
 * KEY CONCEPTS:
 * 
 * 1. Abstract Socket Addressing:
 *    - Address starts with null byte (\0) to indicate abstract namespace
 *    - No filesystem path resolution or permissions checking
 *    - Socket exists only in kernel memory, not on filesystem
 *    - Name is automatically available across all mount namespaces
 * 
 * 2. Connection Process:
 *    - Client creates socket with socket(AF_UNIX, SOCK_STREAM, 0)
 *    - Sets up sockaddr_un with null-prefixed abstract name
 *    - Connects directly without filesystem path resolution
 *    - Standard stream socket communication follows
 * 
 * 3. Comparison with Filesystem Sockets:
 *    - No need to check if socket file exists
 *    - No file permission errors (EACCES)
 *    - No path traversal or directory permission issues
 *    - Faster connection establishment (no filesystem I/O)
 * 
 * 4. Error Handling:
 *    - ECONNREFUSED: Server not listening or abstract name not bound
 *    - ENOENT: Not applicable (no filesystem path to resolve)
 *    - Standard socket errors apply for communication phase
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define ABSTRACT_SOCKET_NAME "\0abstract_socket_demo"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    const char *message;
    ssize_t bytes_sent, bytes_received;
    
    if (argc != 2) {
        printf("=== Unix Domain Socket Abstract Namespace Client ===\n");
        printf("Usage: %s <message>\n", argv[0]);
        printf("Example: %s \"Hello Abstract Server!\"\n", argv[0]);
        exit(1);
    }
    
    message = argv[1];
    
    printf("=== Unix Domain Socket Abstract Namespace Client ===\n");
    printf("Target server: @abstract_socket_demo (abstract namespace)\n");
    printf("Message to send: %s\n\n", message);
    
    /*
     * Create Unix domain socket for abstract namespace connection
     */
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("ERROR: socket() failed");
        exit(1);
    }
    printf("✓ Abstract namespace socket created (fd=%d)\n", client_fd);
    
    /*
     * Setup server address for abstract namespace
     * 
     * CRITICAL: Must specify the exact same abstract name that the
     * server bound to. The leading null byte is essential.
     * 
     * Abstract Address Format:
     * ┌─────────────────────────────────────────────┐
     * │            struct sockaddr_un               │
     * ├─────────────────────────────────────────────┤
     * │ sun_family = AF_UNIX                        │
     * │ sun_path[0] = '\0'  (null byte indicator)   │
     * │ sun_path[1..n] = "abstract_socket_demo"     │
     * └─────────────────────────────────────────────┘
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    
    // Copy the abstract socket name (including the leading null byte)
    memcpy(server_addr.sun_path, ABSTRACT_SOCKET_NAME, 
           strlen(ABSTRACT_SOCKET_NAME + 1) + 1);
    
    /*
     * Connect to server in abstract namespace
     * 
     * This will fail if:
     * - Server is not running (ECONNREFUSED)
     * - Abstract name is not bound (ECONNREFUSED)
     * - System doesn't support abstract namespace (ENOENT - rare)
     * 
     * Note: No filesystem operations involved, so no EACCES errors
     */
    printf("Connecting to abstract namespace server...\n");
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: connect() failed");
        printf("\nTroubleshooting:\n");
        printf("- Is the abstract namespace server running?\n");
        printf("- Check abstract sockets: ss -x | grep '@abstract_socket_demo'\n");
        printf("- Verify Linux kernel supports abstract namespace\n");
        close(client_fd);
        exit(1);
    }
    printf("✓ Connected to abstract namespace server\n\n");
    
    /*
     * Send message to server
     * 
     * Communication works exactly the same as filesystem sockets
     * once the connection is established.
     */
    printf("Sending message (%zu bytes)...\n", strlen(message));
    bytes_sent = send(client_fd, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("ERROR: send() failed");
        close(client_fd);
        exit(1);
    }
    printf("✓ Sent %zd bytes to abstract namespace server\n", bytes_sent);
    
    /*
     * Receive response from server
     */
    printf("Waiting for server response...\n");
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1) {
        perror("ERROR: recv() failed");
    } else if (bytes_received == 0) {
        printf("Server closed connection (no response)\n");
    } else {
        buffer[bytes_received] = '\0';
        printf("✓ Received response (%zd bytes): %s\n", bytes_received, buffer);
    }
    
    /*
     * Clean up and exit
     * 
     * Note: No filesystem cleanup needed for abstract namespace!
     * The socket address automatically disappears when all references are closed.
     */
    close(client_fd);
    printf("\n✓ Connection closed\n");
    printf("Client finished successfully\n");
    printf("Note: Abstract namespace requires no cleanup!\n");
    
    return 0;
}
