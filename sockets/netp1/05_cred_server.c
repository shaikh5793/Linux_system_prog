/*
 * Unix Domain Socket Credential Passing Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UNIX DOMAIN SOCKET CREDENTIAL PASSING - SERVER
 * 
 * This example demonstrates credential passing over Unix domain sockets,
 * a powerful security feature that allows the server to authenticate
 * clients based on their process credentials (PID, UID, GID).
 * 
 * KEY CONCEPTS:
 * 
 * 1. SO_PASSCRED Socket Option:
 *    - Linux-specific socket option for Unix domain sockets
 *    - Enables automatic credential passing with each message
 *    - Must be set on both server and client sockets
 * 
 * 2. Ancillary Data (Control Messages):
 *    - Credentials are passed as "ancillary data" alongside the main message
 *    - Uses control message headers (cmsghdr) to structure the data
 *    - Separate from the main message payload
 * 
 * 3. recvmsg() vs recv():
 *    - recvmsg() can receive both message data and ancillary data
 *    - recv() only receives the main message data
 *    - Required for credential passing
 * 
 * 4. Authentication Logic:
 *    - Server can implement custom authentication based on credentials
 *    - Common patterns: same-user access, root privileges, group membership
 *    - More secure than relying on file permissions alone
 * 
 * 5. Security Benefits:
 *    - Kernel-verified credentials (cannot be spoofed by userspace)
 *    - Process-level authentication and authorization
 *    - Fine-grained access control
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>  // For socket(), bind(), listen(), accept(), setsockopt()
#include <sys/un.h>      // For Unix domain socket structures (sockaddr_un)
#include <errno.h>
#include <signal.h>
#include <pwd.h>         // For getpwuid() to get username from UID

#define SOCKET_PATH "/tmp/credential_passing_socket"
#define BUFFER_SIZE 1024

static int listen_fd = -1;

void cleanup_and_exit(int sig) {
    (void)sig;  /* Suppress unused parameter warning */
    if (listen_fd != -1) {
        close(listen_fd);
    }
    unlink(SOCKET_PATH);
    printf("\nCredential passing server shutdown complete\n");
    exit(0);
}

void print_credentials(struct ucred *cred) {
    struct passwd *pwd;
    
    printf("Client credentials:\n");
    printf("  PID: %d\n", cred->pid);
    printf("  UID: %d", cred->uid);
    
    pwd = getpwuid(cred->uid);
    if (pwd) {
        printf(" (%s)", pwd->pw_name);
    }
    printf("\n");
    printf("  GID: %d\n", cred->gid);
}

int main() {
    struct sockaddr_un server_addr, client_addr;
    int conn_fd;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int optval = 1;
    
    /*
     * Message reception structures for receiving credentials:
     * - msghdr: Main message header structure for recvmsg()
     * - iovec: Describes the main data buffer
     * - cmsghdr: Control message header for ancillary data
     * - ucred: Unix credentials structure (PID, UID, GID)
     * - control_buf: Buffer to store ancillary data (credentials)
     *
     * MESSAGE WITH CREDENTIALS - RECEIVE FORMAT:
     * ┌─────────────────────────────────────────────────────────────┐
     * │                      struct msghdr                          │
     * ├─────────────────────────────────────────────────────────────┤
     * │ msg_name     = NULL (not used for connected sockets)       │
     * │ msg_namelen  = 0                                            │
     * │ msg_iov      → points to iovec array                       │
     * │ msg_iovlen   = 1 (one iovec)                               │
     * │ msg_control  → points to control_buf                       │
     * │ msg_controllen = CMSG_SPACE(sizeof(struct ucred))         │
     * │ msg_flags    = (filled by recvmsg)                         │
     * └─────────────────────────────────────────────────────────────┘
     *                              │
     *                              ▼
     * ┌─────────────────────────────────────────────────────────────┐
     * │                      struct iovec                           │
     * ├─────────────────────────────────────────────────────────────┤
     * │ iov_base     → points to buffer (main message data)        │
     * │ iov_len      = BUFFER_SIZE - 1                              │
     * └─────────────────────────────────────────────────────────────┘
     *                              │
     *                              ▼
     * ┌─────────────────────────────────────────────────────────────┐
     * │                    Main Message Buffer                      │
     * ├─────────────────────────────────────────────────────────────┤
     * │ "Hello with credentials" (null-terminated string)           │
     * └─────────────────────────────────────────────────────────────┘
     *
     *       msg_control points to:
     * ┌─────────────────────────────────────────────────────────────┐
     * │                   Control Buffer                            │
     * │   (contains one or more control message headers)            │
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
     * │ │ │ pid = 1234    (Process ID)                         │ │ │
     * │ │ │ uid = 1000    (User ID)                            │ │ │
     * │ │ │ gid = 1000    (Group ID)                           │ │ │
     * │ │ └─────────────────────────────────────────────────────┘ │ │
     * │ └─────────────────────────────────────────────────────────┘ │
     * └─────────────────────────────────────────────────────────────┘
     */
    struct msghdr msg;
    struct iovec iov;
    struct cmsghdr *cmsg;
    struct ucred *cred;
    char control_buf[CMSG_SPACE(sizeof(struct ucred))];
    
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);
    
    printf("=== Unix Domain Socket Credential Passing Server ===\n");
    printf("Authentication: Process credentials (PID, UID, GID)\n");
    printf("Security: Kernel-verified, cannot be spoofed\n\n");
    
    // Create socket
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket creation failed");
        exit(1);
    }
    
    /*
     * Enable credential passing on the server socket:
     * SO_PASSCRED tells the kernel to include credentials (PID, UID, GID)
     * in the ancillary data when receiving messages via recvmsg().
     * This is a Linux-specific feature for Unix domain sockets.
     */
    if (setsockopt(listen_fd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) == -1) {
        perror("setsockopt SO_PASSCRED failed");
        close(listen_fd);
        exit(1);
    }
    printf("✓ Credential passing enabled on server socket\n");
    
    unlink(SOCKET_PATH);
    
    // Setup and bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(listen_fd);
        exit(1);
    }
    
    if (listen(listen_fd, 5) == -1) {
        perror("listen failed");
        close(listen_fd);
        unlink(SOCKET_PATH);
        exit(1);
    }
    
    printf("✓ Server listening on %s\n", SOCKET_PATH);
    printf("Server PID: %d, UID: %d, GID: %d\n", getpid(), getuid(), getgid());
    printf("Press Ctrl+C to stop\n\n");
    
    while (1) {
        client_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        if (conn_fd == -1) {
            if (errno == EINTR) continue;
            perror("accept failed");
            continue;
        }
        
        printf("=== New client connected ===\n");
        
        /*
         * Enable credential reception for each client connection:
         * This ensures that each individual client connection will
         * include credentials in received messages.
         */
        if (setsockopt(conn_fd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) == -1) {
            perror("setsockopt on client socket failed");
            close(conn_fd);
            continue;
        }
        
        while (1) {
            /*
             * Setup message reception structures:
             * - Clear message header structure
             * - Setup I/O vector for main data buffer
             * - Setup control buffer for ancillary data (credentials)
             * - msg_control points to buffer where credentials will be stored
             * - msg_controllen specifies size of control buffer
             */
            memset(&msg, 0, sizeof(msg));
            iov.iov_base = buffer;                      // Main data buffer
            iov.iov_len = BUFFER_SIZE - 1;             // Size of data buffer
            msg.msg_iov = &iov;                        // Point to I/O vector
            msg.msg_iovlen = 1;                        // Number of I/O vectors
            msg.msg_control = control_buf;             // Control data buffer
            msg.msg_controllen = sizeof(control_buf); // Control buffer size
            
            /*
             * Receive message with credentials:
             * recvmsg() is used instead of recv() because it can receive
             * ancillary data (credentials) along with the main message.
             * The credentials will be placed in the control buffer.
             */
            bytes_received = recvmsg(conn_fd, &msg, 0);
            
            if (bytes_received <= 0) {
                if (bytes_received == 0) {
                    printf("Client disconnected\n");
                } else {
                    perror("recvmsg failed");
                }
                break;
            }
            
            buffer[bytes_received] = '\0';
            printf("Received message: %s\n", buffer);
            
            /*
             * Extract credentials from control message:
             * - Iterate through all control message headers
             * - Look for SOL_SOCKET level with SCM_CREDENTIALS type
             * - Extract ucred structure containing PID, UID, GID
             * - Implement authentication/authorization logic
             * 
             * CMSG_FIRSTHDR: Gets first control message header
             * CMSG_NXTHDR: Gets next control message header
             * CMSG_DATA: Gets data portion of control message
             */
            for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
                if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_CREDENTIALS) {
                    // Extract credentials from control message data
                    cred = (struct ucred*)CMSG_DATA(cmsg);
                    print_credentials(cred);
                    
                    /*
                     * Authentication logic based on credentials:
                     * - Root user (UID 0): Full access
                     * - Same user as server: Access granted
                     * - Different user: Limited access
                     */
                    if (cred->uid == 0) {
                        printf("ROOT USER - Full access granted\n");
                    } else if (cred->uid == getuid()) {
                        printf("SAME USER - Access granted\n");
                    } else {
                        printf("DIFFERENT USER - Limited access\n");
                    }
                    break;
                }
            }
            
            // Send response
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "Message processed by server (PID %d)", getpid());
            if (send(conn_fd, response, strlen(response), 0) == -1) {
                perror("send failed");
                break;
            }
            
            printf("Response sent\n\n");
        }
        
        close(conn_fd);
    }
    
    cleanup_and_exit(0);
    return 0;
}
