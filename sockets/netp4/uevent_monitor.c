/* Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Monitors and displays kernel device events (uevents) using netlink sockets.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <signal.h>

static int sock_fd = -1;
static volatile int running = 1;

void cleanup(int sig) {
    (void)sig;
    running = 0;
    if (sock_fd != -1) close(sock_fd);
    printf("\nDevice monitor stopped.\n");
    exit(0);
}

void parse_uevent(char *data, int len) {
    char *action = NULL;
    char *devpath = NULL;
    char *subsystem = NULL;
    char *devname = NULL;
    
    char *line = data;
    char *end = data + len;
    
    while (line < end) {
        char *next_line = strchr(line, '\0');
        if (!next_line) break;
        
        if (strncmp(line, "ACTION=", 7) == 0) {
            action = line + 7;
        } else if (strncmp(line, "DEVPATH=", 8) == 0) {
            devpath = line + 8;
        } else if (strncmp(line, "SUBSYSTEM=", 10) == 0) {
            subsystem = line + 10;
        } else if (strncmp(line, "DEVNAME=", 8) == 0) {
            devname = line + 8;
        }
        
        line = next_line + 1;
    }
    
    if (action && subsystem) {
        printf("UEVENT: %s %s", action, subsystem);
        if (devname) printf(" (%s)", devname);
        if (devpath) printf(" [%s]", devpath);
        printf("\n");
    }
}

int main() {
    struct sockaddr_nl addr;
    char buffer[4096];
    
    printf("Simple Device Event Monitor (NETLINK_KOBJECT_UEVENT)\n");
    printf("====================================================\n");
    
    signal(SIGINT, cleanup);
    
    /*
     * Step 1: Create the netlink socket.
     * We use SOCK_RAW for NETLINK_KOBJECT_UEVENT.
     * This family is used for receiving kernel uevents, typically related to devices.
     */
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (sock_fd == -1) {
        perror("socket");
        exit(1);
    }
    
    /*
     * Step 2: Bind the socket to a local address.
     * We subscribe to the uevent multicast group by setting nl_groups to 1.
     */
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 1;
    
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sock_fd);
        exit(1);
    }
    
    printf("Monitoring device events... (Press Ctrl+C to stop)\n");
    printf("Generate events: plug/unplug USB devices, etc.\n\n");
    
    /*
     * Step 3: Continuously receive and process uevents.
     * The kernel sends uevents as netlink messages.
     * The payload is a series of null-terminated strings in "KEY=VALUE" format.
     *
     * Packet Layout: [ nlmsghdr | uevent_string ]
     */
    while (running) {
        ssize_t len = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) continue;
        
        buffer[len] = '\0';
        
        /*
         * The first part of the buffer is the netlink message header.
         * The actual uevent data follows.
         * We parse this data to extract useful information.
         */
        char *data = strchr(buffer, '\0');
        if (data && data < buffer + len - 1) {
            data++;
            int remaining = len - (data - buffer);
            if (remaining > 0) {
                parse_uevent(data, remaining);
            }
        }
    }
    
    close(sock_fd);
    return 0;
}
