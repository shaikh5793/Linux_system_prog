/*
 * UDP Discovery Client
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * UDP service discovery client
 * 
 * Demonstrates client-side service discovery using UDP.
 * Client can either actively send discovery requests or passively
 * listen for service announcements from servers.
 * 
 * Key concepts:
 * 
 * 1. Service discovery modes:
 *    - Active: Send broadcast request to find services
 *    - Passive: Listen for periodic service announcements
 *    - Supports both modes for robust discovery
 * 
 * 2. Discovery client flow:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ Create socket → send request (active) or listen (passive) →         │
 *    │ Parse response → display discovered services                         │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. Service parsing:
 *    - Parses service announcement messages
 *    - Extracts service details (name, port, description, etc.)
 *    - Stores and displays discovered services
 * 
 * Assumptions:
 *   - Operates on discovery port 8888
 *   - Can time out while listening for announcements
 *   - Handles multiple services from single announcement
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
#include <sys/select.h>

#define BUFFER_SIZE 1024
#define DISCOVERY_PORT 8888
#define LISTEN_TIMEOUT 5

/*
 * discovery protocol messages
 */
#define MSG_DISCOVERY_REQUEST "DISCOVER"
#define MSG_SERVICE_ANNOUNCE "SERVICE"
#define MSG_SERVICE_RESPONSE "RESPONSE"

/*
 * discovered service structure
 */
struct discovered_service {
    char name[64];
    char server_ip[16];
    int port;
    char description[128];
    char capabilities[256];
    time_t uptime;
    time_t discovered_at;
};

static int client_fd = -1;
static volatile int running = 1;

/*
 * Signal handler for graceful shutdown
 */
void cleanup_and_exit(int sig) {
    (void)sig;
    running = 0;
    if (client_fd != -1) {
        close(client_fd);
    }
    printf("\nservice discovery client shutdown complete\n");
    exit(0);
}

/*
 * Parse service announcement message
 */
int parse_service_message(const char *message, struct discovered_service *services, int max_services) {
    char *msg_copy = strdup(message);
    char *token;
    int service_count = 0;
    
    /*
     * Skip message type and server IP
     */
    token = strtok(msg_copy, "|");
    if (!token) {
        free(msg_copy);
        return 0;
    }
    
    token = strtok(NULL, "|");
    if (!token) {
        free(msg_copy);
        return 0;
    }
    strncpy(services[0].server_ip, token, sizeof(services[0].server_ip) - 1);
    
    token = strtok(NULL, "|");
    if (!token) {
        free(msg_copy);
        return 0;
    }
    
    /*
     * Parse each service entry
     */
    while ((token = strtok(NULL, "|")) != NULL && service_count < max_services) {
        char name[64], description[128], capabilities[256];
        int port;
        long uptime;
        
        /*
         * Parse service format
         */
        if (sscanf(token, "%63[^:]:%d:%127[^:]:%255[^:]:%ld", 
                  name, &port, description, capabilities, &uptime) == 5) {
            
            strncpy(services[service_count].name, name, sizeof(services[service_count].name) - 1);
            services[service_count].port = port;
            strncpy(services[service_count].description, description, sizeof(services[service_count].description) - 1);
            strncpy(services[service_count].capabilities, capabilities, sizeof(services[service_count].capabilities) - 1);
            services[service_count].uptime = uptime;
            services[service_count].discovered_at = time(NULL);
            
            strncpy(services[service_count].server_ip, services[0].server_ip, sizeof(services[service_count].server_ip) - 1);
            
            service_count++;
        }
    }
    
    free(msg_copy);
    return service_count;
}

/*
 * Display discovered services
 */
void display_services(struct discovered_service *services, int count, const char *source) {
    time_t now = time(NULL);
    
    printf("\n=== services discovered from %s ===\n", source);
    printf("discovery time: %s", ctime(&now));
    printf("number of services: %d\n\n", count);
    
    for (int i = 0; i < count; i++) {
        printf("service %d:\n", i + 1);
        printf("  name: %s\n", services[i].name);
        printf("  address: %s:%d\n", services[i].server_ip, services[i].port);
        printf("  description: %s\n", services[i].description);
        printf("  capabilities: %s\n", services[i].capabilities);
        printf("  uptime: %ld seconds\n", services[i].uptime);
        printf("\n");
    }
    
    if (count == 0) {
        printf("no services discovered.\n\n");
    }
}

/*
 * Send discovery request to network
 */
int send_discovery_request() {
    struct sockaddr_in broadcast_addr;
    int broadcast_enable = 1;
    
    /*
     * Enable broadcast on socket
     */
    if (setsockopt(client_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("setsockopt SO_BROADCAST failed");
        return -1;
    }
    
    /*
     * Set up broadcast address
     */
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr("127.255.255.255"); // local broadcast
    broadcast_addr.sin_port = htons(DISCOVERY_PORT);
    
    /*
     * Send discovery request
     */
    const char *request = MSG_DISCOVERY_REQUEST;
    ssize_t bytes_sent = sendto(client_fd, request, strlen(request), 0,
                               (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
    
    if (bytes_sent == -1) {
        perror("failed to send discovery request");
        return -1;
    }
    
    printf("discovery request sent (%zd bytes)\n", bytes_sent);
    return 0;
}

/*
 * Listen for service announcements from servers
 */
void listen_for_services(int timeout_seconds) {
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    char buffer[BUFFER_SIZE];
    time_t start_time = time(NULL);
    
    printf("listening for service announcements (timeout: %d seconds)...\n", timeout_seconds);
    
    while (running && (time(NULL) - start_time) < timeout_seconds) {
        /*
         * Use select for timeout
         */
        fd_set read_fds;
        struct timeval timeout;
        
        FD_ZERO(&read_fds);
        FD_SET(client_fd, &read_fds);
        
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(client_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (activity > 0 && FD_ISSET(client_fd, &read_fds)) {
            memset(buffer, 0, BUFFER_SIZE);
            
            ssize_t bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0,
                                            (struct sockaddr*)&sender_addr, &sender_len);
            
            if (bytes_received > 0) {
                struct discovered_service services[10];
                int count = parse_service_message(buffer, services, 10);
                
                char sender_ip[16];
                inet_ntop(AF_INET, &sender_addr.sin_addr, sender_ip, sizeof(sender_ip));
                
                display_services(services, count, sender_ip);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int interactive_mode = 0;
    
    if (argc > 1 && strcmp(argv[1], "interactive") == 0) {
        interactive_mode = 1;
    }
    
    /*
     * Set up signal handler
     */
    signal(SIGINT, cleanup_and_exit);
    
    /*
     * Create UDP socket
     */
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("socket creation failed");
        exit(1);
    }
    
    if (interactive_mode) {
        char command[64];
        printf("interactive mode - commands: discover, listen, quit\n");
        
        while (running) {
            printf("discovery> ");
            scanf("%63s", command);
            
            if (strcmp(command, "discover") == 0) {
                send_discovery_request();
                listen_for_services(LISTEN_TIMEOUT); 
            } else if (strcmp(command, "listen") == 0) {
                listen_for_services(10);
            } else if (strcmp(command, "quit") == 0) {
                break;
            }
        }
    } else {
        /*
         * Default behavior: discover and listen
         */
        send_discovery_request();
        listen_for_services(LISTEN_TIMEOUT);
    }
    
    close(client_fd);
    return 0;
}
