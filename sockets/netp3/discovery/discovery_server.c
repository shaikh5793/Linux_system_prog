/*
 * UDP Discovery Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UDP service discovery server
 * 
 * Demonstrates service discovery pattern using UDP broadcasting.
 * Server announces available services periodically and responds
 * to discovery requests from clients.
 * 
 * Key concepts:
 * 
 * 1. Service discovery pattern:
 *    - Server announces services via broadcast
 *    - Clients can discover services by sending requests
 *    - Maintains registry of available services
 * 
 * 2. Discovery flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ Register services → periodic broadcast → handle discovery requests    │
 *    │ → Send service list → clients discover services                      │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. Message types:
 *    - "DISCOVER" - Client discovery request
 *    - "SERVICE" - Server service announcement  
 *    - "RESPONSE" - Server response to discovery
 * 
 * Assumptions:
 *   - Runs on discovery port 8888
 *   - Broadcasts service announcements every 10 seconds
 *   - Supports multiple services per server
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
#include <time.h>

#define BUFFER_SIZE 1024
#define DISCOVERY_PORT 8888
#define ANNOUNCE_INTERVAL 10
#define MAX_SERVICES 5

/*
 * discovery protocol messages
 */
#define MSG_DISCOVERY_REQUEST "DISCOVER"
#define MSG_SERVICE_ANNOUNCE "SERVICE"
#define MSG_SERVICE_RESPONSE "RESPONSE"

/*
 * service information structure
 */
struct service_info {
    char name[64];
    int port;
    char description[128];
    char capabilities[256];
    time_t start_time;
    int active;
};

static int server_fd = -1;
static struct service_info services[MAX_SERVICES];
static int service_count = 0;
static volatile int running = 1;

/*
 * Signal handler for graceful shutdown
 */
void cleanup_and_exit(int sig) {
    (void)sig;
    running = 0;
    if (server_fd != -1) {
        close(server_fd);
    }
    printf("\nservice discovery server shutdown complete\n");
    exit(0);
}

/*
 * Add service to registry
 */
int add_service(const char *name, int port, const char *description, const char *capabilities) {
    if (service_count >= MAX_SERVICES) {
        return -1;
    }
    
    strncpy(services[service_count].name, name, sizeof(services[service_count].name) - 1);
    services[service_count].port = port;
    strncpy(services[service_count].description, description, sizeof(services[service_count].description) - 1);
    strncpy(services[service_count].capabilities, capabilities, sizeof(services[service_count].capabilities) - 1);
    services[service_count].start_time = time(NULL);
    services[service_count].active = 1;
    
    service_count++;
    return 0;
}

/*
 * Create service announcement message
 */
void create_service_announcement(char *buffer, size_t buffer_size) {
    time_t current_time = time(NULL);
    snprintf(buffer, buffer_size, "%s|%s|%ld", MSG_SERVICE_ANNOUNCE, "127.0.0.1", current_time);
    
    /*
     * Add each active service
     */
    for (int i = 0; i < service_count; i++) {
        if (services[i].active) {
            char service_entry[256];
            snprintf(service_entry, sizeof(service_entry),
                    "|%.50s:%d:%.50s:%.50s:%ld",
                    services[i].name,
                    services[i].port,
                    services[i].description,
                    services[i].capabilities,
                    current_time - services[i].start_time);
            
            /*
             * Check if adding this service would overflow the buffer
             */
            if (strlen(buffer) + strlen(service_entry) < buffer_size - 1) {
                strcat(buffer, service_entry);
            }
        }
    }
}

/*
 * Broadcast service announcement to network
 */
void broadcast_service_announcement() {
    char announcement[BUFFER_SIZE];
    struct sockaddr_in broadcast_addr;
    int broadcast_enable = 1;
    
    create_service_announcement(announcement, sizeof(announcement));
    
    /*
     * Enable broadcast on socket
     */
    if (setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("setsockopt SO_BROADCAST failed");
        return;
    }
    
    /*
     * Set up broadcast address
     */
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr("127.255.255.255");  // local broadcast
    broadcast_addr.sin_port = htons(DISCOVERY_PORT);
    
    /*
     * Send announcement
     */
    ssize_t bytes_sent = sendto(server_fd, announcement, strlen(announcement), 0,
                               (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
    
    if (bytes_sent > 0) {
        printf("service announcement broadcast (%zd bytes, %d services)\n", bytes_sent, service_count);
    } else {
        perror("failed to broadcast service announcement");
    }
}

/*
 * Handle discovery request from client
 */
void handle_discovery_request(struct sockaddr_in *client_addr) {
    char response[BUFFER_SIZE];
    
    create_service_announcement(response, sizeof(response));
    
    /*
     * Change message type to RESPONSE
     */
    if (strstr(response, MSG_SERVICE_ANNOUNCE) == response) {
        memcpy(response, MSG_SERVICE_RESPONSE, strlen(MSG_SERVICE_RESPONSE));
    }
    
    /*
     * Send response directly to requesting client
     */
    ssize_t bytes_sent = sendto(server_fd, response, strlen(response), 0,
                               (struct sockaddr*)client_addr, sizeof(*client_addr));
    
    if (bytes_sent > 0) {
        printf("discovery response sent to %s:%d (%zd bytes)\n",
               inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), bytes_sent);
    } else {
        perror("failed to send discovery response");
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char *service_name;
    int service_port;
    time_t last_announcement = 0;
    
    /*
     * Check command line arguments
     */
    if (argc != 3) {
        printf("Usage: %s <service_name> <service_port>\n", argv[0]);
        printf("Example: %s \"FileServer\" 9000\n", argv[0]);
        exit(1);
    }
    
    service_name = argv[1];
    service_port = atoi(argv[2]);
    
    if (service_port <= 0 || service_port > 65535) {
        printf("error: service port must be between 1 and 65535\n");
        exit(2);
    }
    
    /*
     * Initialize services array
     */
    memset(services, 0, sizeof(services));
    
    /*
     * Add the main service
     */
    if (add_service(service_name, service_port, "primary service", "file-transfer,messaging") == -1) {
        printf("error: failed to add service\n");
        exit(3);
    }
    
    /*
     * Add some example additional services
     */
    add_service("StatusService", service_port + 1, "system status monitoring", "monitoring,health-check");
    add_service("ConfigService", service_port + 2, "configuration management", "config,settings");
    
    printf("UDP discovery server starting with service '%s' on port %d\n", service_name, service_port);
    printf("discovery server listening on port %d\n", DISCOVERY_PORT);
    
    /*
     * Set up signal handler
     */
    signal(SIGINT, cleanup_and_exit);
    
    /*
     * Create UDP socket
     */
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket creation failed");
        exit(4);
    }
    
    /*
     * Configure server address
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DISCOVERY_PORT);
    
    /*
     * Bind socket
     */
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        exit(5);
    }
    
    printf("registered %d services for discovery\n", service_count);
    printf("broadcasting announcements every %d seconds\n", ANNOUNCE_INTERVAL);
    
    /*
     * Main server loop
     */
    while (running) {
        time_t current_time = time(NULL);
        
        /*
         * Periodic service announcements
         */
        if (current_time - last_announcement >= ANNOUNCE_INTERVAL) {
            broadcast_service_announcement();
            last_announcement = current_time;
        }
        
        /*
         * Check for discovery requests (non-blocking)
         */
        fd_set read_fds;
        struct timeval timeout = {1, 0}; // 1 second timeout
        
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        
        int select_result = select(server_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (select_result > 0 && FD_ISSET(server_fd, &read_fds)) {
            /*
             * Receive discovery request
             */
            ssize_t bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                            (struct sockaddr*)&client_addr, &client_len);
            
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                
                if (strncmp(buffer, MSG_DISCOVERY_REQUEST, strlen(MSG_DISCOVERY_REQUEST)) == 0) {
                    printf("discovery request from %s:%d\n", 
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    handle_discovery_request(&client_addr);
                }
            }
        }
    }
    
    return 0;
}
