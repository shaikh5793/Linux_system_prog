/*
 * UDP Event Notification Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UDP event notification server
 * 
 * Demonstrates event-driven UDP communication pattern.
 * Server broadcasts events to multiple registered clients.
 * Clients can subscribe/unsubscribe to specific event types.
 * 
 * Key concepts:
 * 
 * 1. Event notification pattern:
 *    - Server maintains list of registered clients
 *    - Broadcasts events to all registered clients
 *    - Clients can filter events by type
 * 
 * 2. Event flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ Client registers → server adds to list → event occurs →             │
 *    │ Server broadcasts → clients receive → clients process event         │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. Message types:
 *    - "register" - Client registration
 *    - "unregister" - Client deregistration  
 *    - "event:type:data" - Event broadcast
 * 
 * Assumptions:
 *   - Server listens on port specified by command line
 *   - Maintains client registry for event broadcasting
 *   - Generates periodic test events for demonstration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 50
#define EVENT_INTERVAL 5

typedef struct {
    struct sockaddr_in addr;
    int active;
    time_t last_seen;
} client_t;

static client_t clients[MAX_CLIENTS];
static int client_count = 0;
static int server_running = 1;

void signal_handler(int sig) {
    (void)sig;
    printf("\nShutting down event server...\n");
    server_running = 0;
}

/*
 * Add client to event notification list
 */
int add_client(struct sockaddr_in addr) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].addr.sin_addr.s_addr == addr.sin_addr.s_addr &&
            clients[i].addr.sin_port == addr.sin_port) {
            clients[i].active = 1;
            clients[i].last_seen = time(NULL);
            return i;
        }
    }
    
    if (client_count < MAX_CLIENTS) {
        clients[client_count].addr = addr;
        clients[client_count].active = 1;
        clients[client_count].last_seen = time(NULL);
        return client_count++;
    }
    
    return -1;
}

/*
 * Remove client from event notification list
 */
void remove_client(struct sockaddr_in addr) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].addr.sin_addr.s_addr == addr.sin_addr.s_addr &&
            clients[i].addr.sin_port == addr.sin_port) {
            clients[i].active = 0;
            printf("Client %s:%d unregistered\n", 
                   inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            break;
        }
    }
}

/*
 * Broadcast event to all registered clients
 */
void broadcast_event(int sockfd, const char* event_type, const char* event_data) {
    char event_msg[BUFFER_SIZE];
    snprintf(event_msg, BUFFER_SIZE, "event:%s:%s", event_type, event_data);
    
    int sent_count = 0;
    for (int i = 0; i < client_count; i++) {
        if (clients[i].active) {
            if (sendto(sockfd, event_msg, strlen(event_msg), 0,
                      (struct sockaddr*)&clients[i].addr, 
                      sizeof(clients[i].addr)) > 0) {
                sent_count++;
            }
        }
    }
    
    printf("Event broadcast: %s - sent to %d clients\n", event_msg, sent_count);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    int port;
    ssize_t bytes_received;
    time_t last_event_time = 0;
    int event_counter = 1;
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("Example: %s 8002\n", argv[0]);
        exit(1);
    }
    
    port = atoi(argv[1]);
    signal(SIGINT, signal_handler);
    
    /*
     * Create UDP socket
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(1);
    }
    
    /*
     * Configure server address
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    /*
     * Bind socket
     */
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(2);
    }
    
    printf("UDP event server listening on port %d\n", port);
    printf("Clients can register for event notifications\n");
    
    /*
     * Main server loop
     */
    client_len = sizeof(client_addr);
    
    while (server_running) {
        /*
         * Set socket to non-blocking for periodic event generation
         */
        struct timeval timeout = {1, 0}; /* 1 second timeout */
        fd_set read_fds;
        
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        
        int select_result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (select_result > 0 && FD_ISSET(sockfd, &read_fds)) {
            /*
             * Receive client message
             */
            bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                    (struct sockaddr*)&client_addr, &client_len);
            
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                
                if (strncmp(buffer, "register", 8) == 0) {
                    /*
                     * Client registration
                     */
                    int client_index = add_client(client_addr);
                    if (client_index >= 0) {
                        printf("Client %s:%d registered for events\n",
                               inet_ntoa(client_addr.sin_addr),
                               ntohs(client_addr.sin_port));
                        
                        const char* ack = "registered";
                        sendto(sockfd, ack, strlen(ack), 0,
                              (struct sockaddr*)&client_addr, client_len);
                    }
                } else if (strncmp(buffer, "unregister", 10) == 0) {
                    /*
                     * Client deregistration
                     */
                    remove_client(client_addr);
                    const char* ack = "unregistered";
                    sendto(sockfd, ack, strlen(ack), 0,
                          (struct sockaddr*)&client_addr, client_len);
                }
            }
        }
        
        /*
         * Generate periodic test events
         */
        time_t current_time = time(NULL);
        if (current_time - last_event_time >= EVENT_INTERVAL) {
            char event_data[100];
            snprintf(event_data, sizeof(event_data), "test_event_%d_at_%ld", 
                    event_counter++, current_time);
            
            broadcast_event(sockfd, "system", event_data);
            last_event_time = current_time;
        }
    }
    
    close(sockfd);
    printf("Event server stopped\n");
    return 0;
}
