/*
 * Threaded TCP Socket Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * threaded tcp socket - server side
 * 
 * this example demonstrates a multi-threaded tcp server that handles
 * multiple clients concurrently using posix threads (pthreads).
 * each client connection is handled in a separate thread.
 * 
 * key concepts:
 * 
 * 1. thread-based concurrency:
 *    - each client gets a dedicated thread
 *    - threads share memory space (unlike fork which creates separate processes)
 *    - more efficient than fork for i/o-bound operations
 * 
 * 2. basic threading model:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ main thread                                                         │
 *    │  ├── socket() + bind() + listen()                                   │
 *    │  └── accept() loop ──┐                                              │
 *    │                      │                                              │
 *    │  ┌───────────────────┼────────────────────────────────────────────┐ │
 *    │  │ worker thread 1   │  worker thread 2   │  worker thread n      │ │
 *    │  │  ├── recv/send    │   ├── recv/send    │   ├── recv/send       │ │
 *    │  │  └── close()      │   └── close()      │   └── close()         │ │
 *    │  └───────────────────┴────────────────────┴───────────────────────┘ │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. key functions:
 *    - pthread_create(): create new thread
 *    - pthread_detach(): allow thread to clean up automatically
 * 
 * assumptions:
 *   - multiple clients can connect simultaneously
 *   - each client is handled in its own thread
 *   - server runs on localhost, port specified by user
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

static int listen_fd = -1;

/*
 * structure to pass data to thread
 */
typedef struct {
    int conn_fd;
    struct sockaddr_in client_addr;
} client_data_t;

void cleanup_and_exit(int sig) {
    (void)sig;
    if (listen_fd != -1) {
        close(listen_fd);
    }
    printf("\nThreaded TCP server stopped\n");
    exit(0);
}

/*
 * thread function - client handler
 * this function runs in a separate thread for each client
 */
void* handle_client(void* arg) {
    client_data_t* client_data = (client_data_t*)arg;
    char buffer[BUFFER_SIZE];
    char client_ip[INET_ADDRSTRLEN];
    ssize_t bytes_received;
    pthread_t thread_id = pthread_self();
    
    /*
     * get client information
     */
    inet_ntop(AF_INET, &client_data->client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    
    printf("[Thread %lu] Client connected from %s:%d\n",
           (unsigned long)thread_id, client_ip, ntohs(client_data->client_addr.sin_port));
    
    /*
     * send welcome message
     */
    char welcome[BUFFER_SIZE];
    snprintf(welcome, sizeof(welcome), "Hello from thread %lu", (unsigned long)thread_id);
    send(client_data->conn_fd, welcome, strlen(welcome), 0);
    
    /*
     * message processing loop
     */
    while (1) {
        bytes_received = recv(client_data->conn_fd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            printf("[Thread %lu] Client %s disconnected\n", (unsigned long)thread_id, client_ip);
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("[Thread %lu] Received: %s", (unsigned long)thread_id, buffer);
        
        /*
         * echo response
         * limit buffer to prevent truncation warning
         */
        char response[BUFFER_SIZE];
        char limited_buffer[900];  /* Leave room for thread ID prefix */
        strncpy(limited_buffer, buffer, sizeof(limited_buffer) - 1);
        limited_buffer[sizeof(limited_buffer) - 1] = '\0';
        snprintf(response, sizeof(response), "[Thread %lu] Echo: %s", (unsigned long)thread_id, limited_buffer);
        send(client_data->conn_fd, response, strlen(response), 0);
    }
    
    /*
     * cleanup
     */
    close(client_data->conn_fd);
    free(client_data);
    printf("[Thread %lu] Thread terminating\n", (unsigned long)thread_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    int conn_fd;
    socklen_t client_len;
    int port;
    int optval = 1;
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("Example: %s 8082\n", argv[0]);
        exit(1);
    }
    
    port = atoi(argv[1]);
    
    /*
     * signal handler setup
     */
    signal(SIGINT, cleanup_and_exit);
    
    /*
     * socket creation
     */
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket creation failed");
        exit(1);
    }
    
    /*
     * socket options
     */
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    /*
     * address configuration
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);
    
    /*
     * bind and listen
     */
    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(listen_fd);
        exit(1);
    }
    
    if (listen(listen_fd, 10) == -1) {
        perror("listen failed");
        close(listen_fd);
        exit(1);
    }
    
    printf("Threaded TCP server listening on 127.0.0.1:%d\n", port);
    printf("Each client will be handled in a separate thread\n");
    
    /*
     * main accept loop
     */
    while (1) {
        client_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (conn_fd == -1) {
            if (errno == EINTR) continue;
            perror("accept failed");
            continue;
        }
        
        /*
         * prepare client data for thread
         */
        client_data_t* client_data = malloc(sizeof(client_data_t));
        if (!client_data) {
            close(conn_fd);
            continue;
        }
        
        client_data->conn_fd = conn_fd;
        client_data->client_addr = client_addr;
        
        /*
         * create thread for client
         */
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_data) != 0) {
            perror("pthread_create failed");
            close(conn_fd);
            free(client_data);
            continue;
        }
        
        /*
         * detach thread
         * thread will clean up automatically when it finishes
         */
        pthread_detach(thread_id);
    }
    
    return 0;
}
