/*
 * Simple UDP Message Queue Server
 * Implements a basic FIFO queue using UDP sockets
 * 
 * Commands:
 *   send:message - Add message to queue
 *   get         - Remove and return next message
 *   status      - Show queue count
 * 
 * Responses:
 *   OK          - Message queued successfully
 *   FULL        - Queue is full
 *   msg:content - Message retrieved
 *   EMPTY       - No messages in queue
 *   count/max   - Queue status
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_QUEUE_SIZE 10
#define MAX_MESSAGE_SIZE 256

static char queue[MAX_QUEUE_SIZE][MAX_MESSAGE_SIZE];
static int queue_head = 0;
static int queue_tail = 0;
static int queue_count = 0;
static int server_running = 1;

void signal_handler(int sig) {
    (void)sig;
    printf("\nShutting down server...\n");
    server_running = 0;
}

int enqueue_message(const char* message) {
    if (queue_count >= MAX_QUEUE_SIZE) {
        return -1;
    }
    
    strncpy(queue[queue_tail], message, MAX_MESSAGE_SIZE - 1);
    queue[queue_tail][MAX_MESSAGE_SIZE - 1] = '\0';
    queue_tail = (queue_tail + 1) % MAX_QUEUE_SIZE;
    queue_count++;
    
    return 0;
}

int dequeue_message(char* message) {
    if (queue_count == 0) {
        return -1;
    }
    
    strcpy(message, queue[queue_head]);
    queue_head = (queue_head + 1) % MAX_QUEUE_SIZE;
    queue_count--;
    
    return 0;
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int port;
    ssize_t bytes_received;
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    port = atoi(argv[1]);
    signal(SIGINT, signal_handler);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    
    printf("Queue server listening on port %d\n", port);
    printf("Commands: send:message, get, status\n");
    
    client_len = sizeof(client_addr);
    
    while (server_running) {
        bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes_received <= 0) continue;
        
        buffer[bytes_received] = '\0';
        
        if (strncmp(buffer, "send:", 5) == 0) {
            const char* message = buffer + 5;
            
            if (enqueue_message(message) == 0) {
                strcpy(response, "OK");
                printf("Queued: %s\n", message);
            } else {
                strcpy(response, "FULL");
                printf("Queue full\n");
            }
            
        } else if (strcmp(buffer, "get") == 0) {
            char message[MAX_MESSAGE_SIZE];
            
            if (dequeue_message(message) == 0) {
                snprintf(response, BUFFER_SIZE, "msg:%s", message);
                printf("Dequeued: %s\n", message);
            } else {
                strcpy(response, "EMPTY");
                printf("Queue empty\n");
            }
            
        } else if (strcmp(buffer, "status") == 0) {
            snprintf(response, BUFFER_SIZE, "%d/%d", queue_count, MAX_QUEUE_SIZE);
            printf("Status: %d messages\n", queue_count);
                   
        } else {
            strcpy(response, "ERROR");
            printf("Unknown command: %s\n", buffer);
        }
        
        sendto(sockfd, response, strlen(response), 0,
              (struct sockaddr*)&client_addr, client_len);
    }
    
    close(sockfd);
    return 0;
}
