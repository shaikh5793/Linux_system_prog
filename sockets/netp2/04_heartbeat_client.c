/*
 * TCP Heartbeat Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * tcp heartbeat client
 * 
 * connects to heartbeat server and sends periodic "heartbeat" messages.
 * demonstrates client-side connection monitoring and keep-alive mechanism.
 * 
 * key concepts:
 * 
 * 1. heartbeat client flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ connect → receive welcome → send heartbeat → receive ack → repeat   │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 2. keep-alive mechanism:
 *    - sends "heartbeat" message every 10 seconds
 *    - expects "heartbeat-ack" response from server
 *    - detects server disconnection or timeout
 * 
 * new concepts (not in previous examples):
 * - periodic message sending with timer
 * - connection health monitoring from client side
 * - automatic reconnection on failure (optional)
 * 
 * assumptions:
 *   - connects to server address and port from command line
 *   - sends heartbeat every 10 seconds
 *   - exits on server disconnect or user interrupt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define HEARTBEAT_INTERVAL 10  /* seconds */

static int client_running = 1;

void signal_handler(int sig) {
    (void)sig;  /* suppress unused parameter warning */
    printf("\nShutting down heartbeat client...\n");
    client_running = 0;
}

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char *server_ip;
    int server_port;
    ssize_t bytes_received;
    
    /*
     * check command line arguments
     */
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        printf("Example: %s 127.0.0.1 8083\n", argv[0]);
        exit(1);
    }
    
    server_ip = argv[1];
    server_port = atoi(argv[2]);
    
    /*
     * setup signal handling
     */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    /*
     * create socket and connect to server
     */
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket creation failed");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid server IP address\n");
        close(client_fd);
        exit(1);
    }
    
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection to server failed");
        close(client_fd);
        exit(2);
    }
    
    printf("Connected to heartbeat server %s:%d\n", server_ip, server_port);
    
    /*
     * receive welcome message from server
     */
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Server welcome: %s", buffer);
    }
    
    /*
     * main heartbeat loop - core of keep-alive mechanism
     * 
     * this loop implements the client-side heartbeat protocol:
     * 1. send "heartbeat\n" message to server
     * 2. wait for "heartbeat-ack\n" response with timeout
     * 3. sleep for HEARTBEAT_INTERVAL seconds
     * 4. repeat until user interruption or connection failure
     * 
     * the heartbeat serves multiple purposes:
     * - proves client is still alive and responsive
     * - detects server failures (no ack received)
     * - maintains connection through firewalls/NAT
     * - provides regular communication for health monitoring
     * 
     * failure conditions that break the loop:
     * - send() fails (server disconnected)
     * - select() timeout (server not responding)
     * - recv() fails (connection lost)
     * - user interruption (SIGINT/SIGTERM)
     */
    while (client_running) {
        /*
         * step 1: send heartbeat message to server
         * 
         * the message "heartbeat\n" is a simple text protocol
         * that the server recognizes as a keep-alive signal.
         * 
         * note: sending exactly 10 bytes ("heartbeat\n" = 10 chars)
         * this is important for the server's message parsing.
         */
        printf("Sending heartbeat...\n");
        if (send(client_fd, "heartbeat\n", 10, 0) < 0) {
            perror("failed to send heartbeat - server may be down");
            break;  /* exit loop, connection is broken */
        }
        
        /*
         * step 2: wait for server acknowledgment with timeout
         * 
         * this implements timeout-based waiting for the server's response.
         * we use select() instead of blocking recv() to avoid hanging
         * indefinitely if the server is down or not responding.
         * 
         * select() mechanism:
         * 1. creates a file descriptor set containing our socket
         * 2. waits up to 5 seconds for data to become available
         * 3. returns when data arrives, timeout occurs, or error
         * 
         * why timeout is important:
         * - prevents client from hanging if server crashes
         * - allows detection of network problems
         * - provides responsive behavior for user interruption
         * - enables client to decide when to give up
         */
        fd_set read_fds;        /* set of file descriptors to monitor */
        struct timeval timeout; /* timeout specification for select() */
        
        /* initialize the file descriptor set */
        FD_ZERO(&read_fds);               /* clear all descriptors */
        FD_SET(client_fd, &read_fds);     /* add our socket to the set */
        
        /* set timeout to 5 seconds - reasonable for local communication */
        timeout.tv_sec = 5;   /* 5 seconds */
        timeout.tv_usec = 0;  /* 0 microseconds */
        
        /*
         * call select() to wait for data or timeout
         * 
         * parameters:
         * - client_fd + 1: highest file descriptor number + 1
         * - &read_fds: set of descriptors to check for reading
         * - NULL: no write descriptors to check
         * - NULL: no exception descriptors to check
         * - &timeout: maximum time to wait
         * 
         * return values:
         * - > 0: number of descriptors ready (data available)
         * - 0: timeout occurred (no data within 5 seconds)
         * - < 0: error occurred
         */
        int select_result = select(client_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        /* handle the three possible outcomes of select() */
        if (select_result < 0) {
            /* system error in select() call */
            perror("select failed");
            break;
        } else if (select_result == 0) {
            /* timeout: no data received within 5 seconds */
            printf("TIMEOUT: Server did not acknowledge heartbeat within 5 seconds\n");
            printf("Server may be down or network connection lost\n");
            break;
        } else {
            /*
             * step 3: data is available, receive the acknowledgment
             * 
             * at this point, select() confirmed that recv() will not block
             * because data is ready to be read from the socket.
             */
            bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            
            /* check for connection closure or error */
            if (bytes_received <= 0) {
                printf("Server disconnected during heartbeat exchange\n");
                break;
            }
            
            /* null-terminate received data for string comparison */
            buffer[bytes_received] = '\0';
            
            /*
             * validate the server's response
             * 
             * expected response: "heartbeat-ack\n"
             * this confirms the server received and processed our heartbeat
             */
            if (strncmp(buffer, "heartbeat-ack", 13) == 0) {
                printf("✓ Heartbeat acknowledged by server\n");
            } else {
                printf("⚠ Unexpected response from server: %s", buffer);
                /* continue anyway - server might send additional data */
            }
        }
        
        /*
         * step 4: wait for next heartbeat interval
         * 
         * sleep for HEARTBEAT_INTERVAL seconds before sending the next heartbeat.
         * this timing is important for the heartbeat protocol:
         * 
         * timing considerations:
         * - too frequent: wastes bandwidth and server resources
         * - too infrequent: delays detection of connection problems
         * - 10 seconds is reasonable for most local applications
         * 
         * during sleep:
         * - client remains connected but inactive
         * - server's timeout monitor continues checking
         * - user can still interrupt with Ctrl+C (signal handler)
         * 
         * note: sleep() is interruptible by signals, so SIGINT/SIGTERM
         * will cause immediate wakeup and loop exit.
         */
        printf("Waiting %d seconds for next heartbeat...\n", HEARTBEAT_INTERVAL);
        sleep(HEARTBEAT_INTERVAL);
    }
    
    /*
     * cleanup
     */
    printf("Closing connection to server\n");
    close(client_fd);
    return 0;
}
