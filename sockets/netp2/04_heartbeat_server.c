/*
 * TCP Heartbeat Server
 * Author: Raghu Bharadwaj
 * Copyright (c) 2024 Techveda. Licensed under MIT.
 */

/*
 * tcp heartbeat server
 * 
 * monitors client connections through periodic heartbeat messages.
 * detects disconnected clients and maintains active connection list.
 * demonstrates connection health monitoring in tcp applications.
 * 
 * key concepts:
 * 
 * 1. heartbeat mechanism:
 *    - clients send periodic "heartbeat" messages
 *    - server tracks last heartbeat time per client
 *    - timeout detection for dead connections
 * 
 * 2. connection monitoring:
 *    ┌─────────────────────────────────────────────────────────────────────┐
 *    │ client connects → server accepts → client sends heartbeats          │
 *    │ server responds → timeout check → cleanup dead connections         │
 *    └─────────────────────────────────────────────────────────────────────┘
 * 
 * 3. threading model:
 *    - main thread: accepts new connections
 *    - client threads: handle individual client heartbeats
 *    - timeout thread: monitors and cleans up dead connections
 * 
 * new concepts (not in previous examples):
 * - client connection tracking with timestamps
 * - timeout monitoring with separate thread
 * - connection health detection
 * - automatic cleanup of dead connections
 * 
 * assumptions:
 *   - listens on port specified by command line
 *   - expects "heartbeat" messages every 10 seconds
 *   - disconnects clients after 30 second timeout
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 50
#define HEARTBEAT_TIMEOUT 30

/*
 * client connection tracking structure
 * 
 * this structure stores all information needed to monitor
 * the health of each connected client:
 * 
 * - socket_fd: the client's socket file descriptor for communication
 * - addr: client's network address information (IP and port)
 * - last_heartbeat: timestamp of the last received heartbeat message
 * - thread_id: pthread handle for the client handler thread
 * - active: flag indicating if this client slot is currently in use
 * 
 * the heartbeat mechanism works by:
 * 1. recording when each heartbeat message arrives (last_heartbeat)
 * 2. having a separate thread periodically check all clients
 * 3. disconnecting clients whose last_heartbeat is too old
 */
typedef struct {
    int socket_fd;            /* file descriptor for client communication */
    struct sockaddr_in addr;  /* client's IP address and port information */
    time_t last_heartbeat;    /* unix timestamp of last received heartbeat */
    pthread_t thread_id;      /* thread handling this specific client */
    int active;               /* 1 = client active, 0 = slot available */
} client_info_t;

static client_info_t clients[MAX_CLIENTS];
static int client_count = 0;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
static int server_running = 1;

void signal_handler(int sig) {
    (void)sig;  /* suppress unused parameter warning */
    printf("\nShutting down heartbeat server...\n");
    server_running = 0;
}

/*
 * add_client() - register a new client for heartbeat monitoring
 * 
 * this function adds a newly connected client to the global clients array
 * and initializes all necessary fields for heartbeat tracking.
 * 
 * thread safety: uses mutex to protect the shared clients array
 * from concurrent access by multiple threads.
 * 
 * parameters:
 * - socket_fd: file descriptor for client communication
 * - addr: client's network address (IP and port)
 * 
 * returns:
 * - client index (0 to MAX_CLIENTS-1) on success
 * - -1 if maximum clients reached
 * 
 * the client will be tracked until:
 * 1. it sends a goodbye message
 * 2. the connection is lost (recv returns <= 0)
 * 3. heartbeat timeout occurs (no heartbeat for HEARTBEAT_TIMEOUT seconds)
 */
int add_client(int socket_fd, struct sockaddr_in addr) {
    /* acquire exclusive access to clients array */
    pthread_mutex_lock(&clients_mutex);
    
    /* check if we have space for another client */
    if (client_count >= MAX_CLIENTS) {
        pthread_mutex_unlock(&clients_mutex);
        printf("WARNING: Maximum clients (%d) reached, rejecting new connection\n", MAX_CLIENTS);
        return -1;
    }
    
    /* find next available slot and initialize client info */
    int index = client_count++;
    clients[index].socket_fd = socket_fd;
    clients[index].addr = addr;
    clients[index].last_heartbeat = time(NULL);  /* record connection time as first "heartbeat" */
    clients[index].active = 1;                   /* mark slot as occupied */
    
    /* release mutex - other threads can now access clients array */
    pthread_mutex_unlock(&clients_mutex);
    
    printf("Client %d registered: %s:%d (total clients: %d)\n", 
           index, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), client_count);
    
    return index;
}

/*
 * remove_client() - safely disconnect and cleanup a client
 * 
 * this function handles the proper cleanup when a client disconnects
 * either voluntarily, due to connection loss, or timeout.
 * 
 * thread safety: uses mutex to ensure only one thread modifies
 * the clients array at a time.
 * 
 * cleanup steps:
 * 1. close the socket connection
 * 2. mark the client slot as inactive
 * 3. print removal notification
 * 
 * note: this function doesn't compact the array - inactive slots
 * remain in place but are marked as inactive for reuse.
 */
void remove_client(int index) {
    /* acquire exclusive access to modify clients array */
    pthread_mutex_lock(&clients_mutex);
    
    /* validate index and ensure client is actually active */
    if (index >= 0 && index < client_count && clients[index].active) {
        printf("Removing client %s:%d (was active for %ld seconds)\n", 
               inet_ntoa(clients[index].addr.sin_addr),
               ntohs(clients[index].addr.sin_port),
               time(NULL) - clients[index].last_heartbeat);
        
        /* close network connection to client */
        close(clients[index].socket_fd);
        
        /* mark this slot as available for reuse */
        clients[index].active = 0;
    }
    
    /* release mutex - other threads can access clients array */
    pthread_mutex_unlock(&clients_mutex);
}

/*
 * handle_client() - thread function for each connected client
 * 
 * this function runs in a separate thread for each client connection.
 * it handles:
 * 1. sending welcome message to client
 * 2. receiving and processing heartbeat messages
 * 3. updating last_heartbeat timestamp for health monitoring
 * 4. sending acknowledgments back to client
 * 5. detecting client disconnections
 * 
 * heartbeat protocol:
 * - client sends "heartbeat\n" message
 * - server updates timestamp and responds with "heartbeat-ack\n"
 * - if no heartbeat received for HEARTBEAT_TIMEOUT seconds, client is disconnected
 * 
 * thread lifecycle:
 * 1. thread created when client connects
 * 2. runs until client disconnects or server shuts down
 * 3. cleanup performed by remove_client()
 * 4. thread automatically exits and is detached
 */
void* handle_client(void* arg) {
    /* extract client index from thread argument */
    int client_index = *(int*)arg;
    free(arg);  /* free the malloc'd argument from main thread */
    
    /* get direct reference to this client's socket */
    int socket_fd = clients[client_index].socket_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    
    /* send welcome message to let client know server is ready */
    const char* welcome = "heartbeat-server-ready\n";
    send(socket_fd, welcome, strlen(welcome), 0);
    printf("Sent welcome message to client %d\n", client_index);
    
    /*
     * main client communication loop
     * continues until:
     * 1. server shutdown (server_running = 0)
     * 2. client marked inactive (timeout or manual removal)
     * 3. socket error or client disconnect
     */
    while (server_running && clients[client_index].active) {
        /* wait for message from client (blocking call) */
        bytes_received = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
        
        /* check for client disconnect or socket error */
        if (bytes_received <= 0) {
            printf("Client %d disconnected or connection lost\n", client_index);
            break;  /* exit loop and cleanup */
        }
        
        /* null-terminate received data for string processing */
        buffer[bytes_received] = '\0';
        
        /*
         * heartbeat message processing
         * 
         * this is the core of the heartbeat mechanism:
         * 1. check if received message is a heartbeat
         * 2. update the client's last_heartbeat timestamp
         * 3. send acknowledgment back to client
         * 
         * thread safety: mutex protects last_heartbeat field
         * since timeout_monitor thread also reads this field
         */
        if (strncmp(buffer, "heartbeat", 9) == 0) {
            /* acquire lock to safely update timestamp */
            pthread_mutex_lock(&clients_mutex);
            clients[client_index].last_heartbeat = time(NULL);  /* record current time */
            pthread_mutex_unlock(&clients_mutex);
            
            printf("Heartbeat received from %s:%d at %ld\n",
                   inet_ntoa(clients[client_index].addr.sin_addr),
                   ntohs(clients[client_index].addr.sin_port),
                   time(NULL));
            
            /* send acknowledgment so client knows heartbeat was received */
            const char* ack = "heartbeat-ack\n";
            send(socket_fd, ack, strlen(ack), 0);
        } else {
            /* handle other types of messages if needed */
            printf("Non-heartbeat message from client %d: %s", client_index, buffer);
        }
    }
    
    /* cleanup when client thread exits */
    printf("Client %d handler thread exiting\n", client_index);
    remove_client(client_index);
    return NULL;
}

/*
 * timeout_monitor() - background thread for heartbeat timeout detection
 * 
 * this function runs in a separate background thread and periodically
 * checks all connected clients to see if any have stopped sending heartbeats.
 * 
 * the timeout mechanism:
 * 1. runs every 5 seconds (configurable check interval)
 * 2. examines each active client's last_heartbeat timestamp
 * 3. calculates time elapsed since last heartbeat
 * 4. disconnects clients that exceed HEARTBEAT_TIMEOUT seconds
 * 
 * why this is necessary:
 * - clients may crash without sending goodbye messages
 * - network connections can be lost silently
 * - server needs to detect and cleanup dead connections
 * - prevents resource leaks from abandoned connections
 * 
 * thread safety:
 * - uses mutex to safely read client data
 * - minimal lock time to avoid blocking client threads
 * - modifies client state only after timeout confirmed
 * 
 * timeout detection algorithm:
 * current_time - last_heartbeat > HEARTBEAT_TIMEOUT
 * 
 * example: if HEARTBEAT_TIMEOUT = 30 seconds
 * - client connects at time 100, last_heartbeat = 100
 * - at time 135, monitor checks: 135 - 100 = 35 > 30
 * - client is disconnected for timeout
 */
void* timeout_monitor(void* arg) {
    (void)arg;  /* suppress unused parameter warning */
    time_t current_time;
    
    printf("Timeout monitor thread started (checking every 5 seconds)\n");
    
    /*
     * main monitoring loop
     * continues until server shutdown signal received
     */
    while (server_running) {
        /* wait 5 seconds between checks to avoid excessive CPU usage */
        sleep(5);
        
        /* get current timestamp for timeout calculations */
        current_time = time(NULL);
        
        /*
         * acquire mutex to safely examine client array
         * this prevents race conditions with client handler threads
         * that are updating last_heartbeat timestamps
         */
        pthread_mutex_lock(&clients_mutex);
        
        /* examine each client slot for timeout conditions */
        for (int i = 0; i < client_count; i++) {
            /* only check clients that are currently active */
            if (clients[i].active) {
                /*
                 * timeout detection calculation:
                 * if (time_since_last_heartbeat > allowed_timeout)
                 * 
                 * this accounts for:
                 * - client crashes (no more heartbeats sent)
                 * - network failures (heartbeats not reaching server)
                 * - client applications hanging or blocked
                 */
                time_t time_since_heartbeat = current_time - clients[i].last_heartbeat;
                
                if (time_since_heartbeat > HEARTBEAT_TIMEOUT) {
                    printf("TIMEOUT: Client %s:%d exceeded %d second limit " 
                           "(no heartbeat for %ld seconds)\n",
                           inet_ntoa(clients[i].addr.sin_addr),
                           ntohs(clients[i].addr.sin_port),
                           HEARTBEAT_TIMEOUT,
                           time_since_heartbeat);
                    
                    /*
                     * cleanup timed-out client:
                     * 1. close the socket connection
                     * 2. mark slot as inactive for reuse
                     * 
                     * note: the client handler thread will detect the
                     * closed socket and exit gracefully
                     */
                    close(clients[i].socket_fd);
                    clients[i].active = 0;
                } else {
                    /* optional: log healthy clients for debugging */
                    printf("Client %s:%d healthy (last heartbeat %ld seconds ago)\n",
                           inet_ntoa(clients[i].addr.sin_addr),
                           ntohs(clients[i].addr.sin_port),
                           time_since_heartbeat);
                }
            }
        }
        
        /* release mutex - client threads can now update timestamps */
        pthread_mutex_unlock(&clients_mutex);
    }
    
    printf("Timeout monitor thread exiting\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    int port;
    pthread_t timeout_thread;
    
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("Example: %s 8083\n", argv[0]);
        exit(1);
    }
    
    port = atoi(argv[1]);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket creation failed");
        exit(1);
    }
    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(2);
    }
    
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(3);
    }
    
    /*
     * NEW: start background timeout monitoring thread
     * runs independently to check client health
     */
    if (pthread_create(&timeout_thread, NULL, timeout_monitor, NULL) != 0) {
        perror("failed to create timeout thread");
        exit(4);
    }
    
    printf("Heartbeat server listening on port %d\n", port);
    printf("Client timeout: %d seconds\n", HEARTBEAT_TIMEOUT);
    
    client_len = sizeof(client_addr);
    while (server_running) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (server_running) {
                perror("accept failed");
            }
            continue;
        }
        
        printf("New client connected: %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
        
        /*
         * NEW: add client to monitoring system
         * registers client for heartbeat tracking
         */
        int client_index = add_client(client_fd, client_addr);
        if (client_index < 0) {
            printf("Maximum clients reached, rejecting connection\n");
            close(client_fd);
            continue;
        }
        
        int* index_arg = malloc(sizeof(int));
        *index_arg = client_index;
        
        if (pthread_create(&clients[client_index].thread_id, NULL, 
                          handle_client, index_arg) != 0) {
            perror("failed to create client thread");
            remove_client(client_index);
            free(index_arg);
        } else {
            pthread_detach(clients[client_index].thread_id);
        }
    }
    
    printf("Cleaning up connections...\n");
    
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].active) {
            close(clients[i].socket_fd);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    close(server_fd);
    pthread_cancel(timeout_thread);
    pthread_join(timeout_thread, NULL);
    
    printf("Heartbeat server shutdown complete\n");
    return 0;
}
