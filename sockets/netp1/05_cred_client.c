/*
 * Unix Domain Socket Credential Passing Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UNIX DOMAIN SOCKET CREDENTIAL PASSING - CLIENT
 * 
 * This example demonstrates how a client sends its credentials along with
 * messages to a server for authentication purposes.
 * 
 * KEY CONCEPTS:
 * 
 * 1. sendmsg() vs send():
 *    - sendmsg() can send both message data and ancillary data
 *    - send() only sends the main message data
 *    - Required for credential passing
 * 
 * 2. Control Message Structure:
 *    - Credentials are packaged in control messages (ancillary data)
 *    - Uses CMSG_* macros to build and access control message headers
 *    - Control data is separate from main message payload
 * 
 * 3. ucred Structure:
 *    - Contains PID, UID, GID of the sending process
 *    - Automatically filled by the kernel (trustworthy)
 *    - Used by server for authentication decisions
 * 
 * 4. Authentication Flow:
 *    - Client prepares credentials and message
 *    - Client sends both using sendmsg()
 *    - Server receives and validates credentials
 *    - Server grants/denies access based on credentials
 * 
 * 5. Use Cases:
 *    - Secure IPC where process identity matters
 *    - Service authentication without passwords
 *    - Fine-grained access control in system services
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pwd.h>

#define SOCKET_PATH "/tmp/credential_passing_socket"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    const char *message;
    ssize_t bytes_sent, bytes_received;
    
    /*
     * Message sending structures for credential passing:
     * - msghdr: Main message structure for sendmsg()
     * - iovec: Describes the data portion of the message
     * - cmsghdr: Control message header for ancillary data
     * - ucred: Unix credentials structure (PID, UID, GID)
     * - control_buf: Buffer to hold the credentials data
     * - passwd: For username lookup
     *
     * MESSAGE WITH CREDENTIALS - SEND FORMAT:
     * ┌─────────────────────────────────────────────────────────────┐
     * │                      struct msghdr                          │
     * ├─────────────────────────────────────────────────────────────┤
     * │ msg_name     = NULL (connected socket, address not needed)  │
     * │ msg_namelen  = 0                                            │
     * │ msg_iov      → points to iovec array                       │
     * │ msg_iovlen   = 1 (one iovec)                               │
     * │ msg_control  → points to control_buf                       │
     * │ msg_controllen = CMSG_SPACE(sizeof(struct ucred))         │
     * │ msg_flags    = 0 (no special flags)                        │
     * └─────────────────────────────────────────────────────────────┘
     *                              │
     *                              ▼
     * ┌─────────────────────────────────────────────────────────────┐
     * │                      struct iovec                           │
     * ├─────────────────────────────────────────────────────────────┤
     * │ iov_base     → points to message string                    │
     * │ iov_len      = strlen(message)                              │
     * └─────────────────────────────────────────────────────────────┘
     *                              │
     *                              ▼
     * ┌─────────────────────────────────────────────────────────────┐
     * │                    Message String Buffer                    │
     * ├─────────────────────────────────────────────────────────────┤
     * │ "Hello with credentials" (variable length, no null term)    │
     * └─────────────────────────────────────────────────────────────┘
     *
     *       msg_control points to:
     * ┌─────────────────────────────────────────────────────────────┐
     * │                   Control Buffer                            │
     * │     (properly aligned for cmsghdr structure)               │
     * ├─────────────────────────────────────────────────────────────┤
     * │                   struct cmsghdr                            │
     * │ ┌─────────────────────────────────────────────────────────┐ │
     * │ │ cmsg_len    = CMSG_LEN(sizeof(struct ucred))           │ │
     * │ │ cmsg_level  = SOL_SOCKET                                │ │
     * │ │ cmsg_type   = SCM_CREDENTIALS                           │ │
     * │ └─────────────────────────────────────────────────────────┘ │
     * │                   Credential Data                           │
     * │ ┌─────────────────────────────────────────────────────────┐ │
     * │ │                  struct ucred                           │ │
     * │ │ ┌─────────────────────────────────────────────────────┐ │ │
     * │ │ │ pid = getpid()  (Current Process ID)               │ │ │
     * │ │ │ uid = getuid()  (Current User ID)                  │ │ │
     * │ │ │ gid = getgid()  (Current Group ID)                 │ │ │
     * │ │ └─────────────────────────────────────────────────────┘ │ │
     * │ └─────────────────────────────────────────────────────────┘ │
     * └─────────────────────────────────────────────────────────────┘
     *
     * CREDENTIAL VERIFICATION FLOW:
     * Client Process ──► Kernel ──► Server Process
     *      │              │              │
     *      │              │              ▼
     *   sendmsg()    Validates &     recvmsg()
     *      │        Attaches Creds      │
     *      │              │              │
     *      ▼              ▼              ▼
     *  ucred struct   Trusted Creds   Authentication
     *  (PID,UID,GID)  (Cannot Spoof)  Decision
     */
    struct msghdr msg;
    struct iovec iov;
    struct cmsghdr *cmsg;
    struct ucred cred;
    char control_buf[CMSG_SPACE(sizeof(struct ucred))];
    struct passwd *pwd;
    
    if (argc != 2) {
        printf("=== Unix Domain Socket Credential Passing Client ===\n");
        printf("Usage: %s <message>\n", argv[0]);
        printf("Example: %s \"Hello with credentials\"\n", argv[0]);
        exit(1);
    }
    
    message = argv[1];
    
    printf("=== Unix Domain Socket Credential Passing Client ===\n");
    printf("Authentication: Sending process credentials with message\n");
    printf("Target server: %s\n\n", SOCKET_PATH);
    
    // Display client info
    printf("Client process credentials:\n");
    printf("  PID: %d\n", getpid());
    printf("  UID: %d", getuid());
    pwd = getpwuid(getuid());
    if (pwd) {
        printf(" (%s)", pwd->pw_name);
    }
    printf("\n");
    printf("  GID: %d\n", getgid());
    printf("\n");
    
    // Create socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket creation failed");
        exit(1);
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Connect to server
    printf("Connecting to credential passing server...\n");
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
        close(client_fd);
        exit(1);
    }
    printf("✓ Connected successfully\n\n");
    
    /*
     * Prepare credentials to send with message:
     * The ucred structure contains three key identifiers:
     * - PID: Process ID of the sending process
     * - UID: User ID of the process owner
     * - GID: Group ID of the process owner
     * These will be validated by the server for authentication
     */
    cred.pid = getpid();  // Current process ID
    cred.uid = getuid();  // Current user ID
    cred.gid = getgid();  // Current group ID
    
    /*
     * Setup message with credentials using sendmsg():
     * This requires setting up both the main message data and
     * the ancillary data (credentials) in separate structures.
     */
    memset(&msg, 0, sizeof(msg));
    
    /*
     * Setup main message data:
     * - iov_base: Points to the actual message data
     * - iov_len: Length of the message data
     * - msg_iov: Points to the I/O vector array
     * - msg_iovlen: Number of I/O vectors (1 in this case)
     */
    iov.iov_base = (void*)message;  // Message data buffer
    iov.iov_len = strlen(message);  // Length of message
    msg.msg_iov = &iov;             // Point to I/O vector
    msg.msg_iovlen = 1;             // One I/O vector
    
    /*
     * Setup control message for credentials:
     * - msg_control: Points to control data buffer
     * - msg_controllen: Size of control data buffer
     * - Control message contains the credentials structure
     */
    msg.msg_control = control_buf;              // Control data buffer
    msg.msg_controllen = sizeof(control_buf);  // Control buffer size
    
    /*
     * Build control message header:
     * - CMSG_FIRSTHDR: Gets pointer to first control message header
     * - cmsg_level: Protocol level (SOL_SOCKET for socket-level)
     * - cmsg_type: Type of control data (SCM_CREDENTIALS)
     * - cmsg_len: Length of control message including header
     * - CMSG_DATA: Gets pointer to control message data area
     */
    cmsg = CMSG_FIRSTHDR(&msg);                        // Get control msg header
    cmsg->cmsg_level = SOL_SOCKET;                     // Socket level
    cmsg->cmsg_type = SCM_CREDENTIALS;                 // Credentials type
    cmsg->cmsg_len = CMSG_LEN(sizeof(struct ucred));  // Total length
    memcpy(CMSG_DATA(cmsg), &cred, sizeof(struct ucred)); // Copy credentials
    
    /*
     * Send message with credentials using sendmsg():
     * sendmsg() is required (instead of send()) because it can
     * transmit ancillary data (credentials) along with the message.
     * The server will receive both the message and credentials.
     */
    printf("Sending message with credentials: %s\n", message);
    bytes_sent = sendmsg(client_fd, &msg, 0);
    if (bytes_sent == -1) {
        perror("sendmsg failed");
        close(client_fd);
        exit(1);
    }
    printf("✓ Sent %zd bytes with credentials\n", bytes_sent);
    
    // Receive response
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1) {
        perror("recv failed");
    } else if (bytes_received == 0) {
        printf("Server closed connection\n");
    } else {
        buffer[bytes_received] = '\0';
        printf("✓ Server response: %s\n", buffer);
    }
    
    // Interactive mode
    printf("\n=== Interactive Mode ===\n");
    printf("Enter messages to send with credentials (type 'quit' to exit):\n");
    
    while (1) {
        printf("\nMessage: ");
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
        
        /*
         * Setup message again for interactive mode:
         * Update the I/O vector to point to new message data.
         * The control message (credentials) remains the same.
         */
        iov.iov_base = buffer;         // Point to new message
        iov.iov_len = strlen(buffer);  // Update message length
        
        /*
         * Send with credentials:
         * Each message sent includes the same credential information,
         * allowing the server to authenticate each message.
         */
        if (sendmsg(client_fd, &msg, 0) == -1) {
            perror("sendmsg failed");
            break;
        }
        
        // Receive response
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Server response: %s\n", buffer);
    }
    
    close(client_fd);
    printf("\n✓ Client disconnected\n");
    return 0;
}
