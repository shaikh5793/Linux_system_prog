/* Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Monitors and displays security audit events using the NETLINK_AUDIT family.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/audit.h>
#include <signal.h>

static int sock_fd = -1;
static volatile int running = 1;

void cleanup(int sig) {
    (void)sig;
    running = 0;
    if (sock_fd != -1) close(sock_fd);
    printf("\nAudit monitor stopped.\n");
    exit(0);
}

const char* get_audit_type(int type) {
    switch (type) {
        case AUDIT_GET: return "GET";
        case AUDIT_SET: return "SET";
        case AUDIT_LOGIN: return "LOGIN";
        case AUDIT_USER: return "USER";
        case AUDIT_DAEMON_START: return "DAEMON_START";
        case AUDIT_DAEMON_END: return "DAEMON_END";
        case AUDIT_SYSCALL: return "SYSCALL";
        default: return "OTHER";
    }
}

int main() {
    struct sockaddr_nl addr;
    char buffer[4096];
    
    printf("Simple Audit Monitor (NETLINK_AUDIT)\n");
    printf("====================================\n");
    
    if (geteuid() != 0) {
        printf("This program requires root privileges.\n");
        printf("Please run with sudo.\n");
        exit(1);
    }
    
    signal(SIGINT, cleanup);
    
    /*
     * Step 1: Create the netlink socket.
     * We use SOCK_RAW for NETLINK_AUDIT.
     * NETLINK_AUDIT is the protocol for the kernel audit subsystem.
     */
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_AUDIT);
    if (sock_fd == -1) {
        perror("socket");
        exit(1);
    }
    
    /*
     * Step 2: Bind the socket to a local address.
     * This is required to start receiving audit events from the kernel.
     * We need to subscribe to the audit multicast group (group 1).
     */
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 1; // Subscribe to audit multicast group
    
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sock_fd);
        exit(1);
    }
    
    printf("Monitoring security audit events... (Press Ctrl+C to stop)\n");
    printf("Generate events with: sudo, su, login attempts, etc.\n\n");
    
    /*
     * Step 3: Continuously receive and process audit events.
     * The kernel sends audit events as netlink messages.
     * Unlike other netlink families, the payload is often a text string.
     *
     * Packet Layout: [ nlmsghdr | audit_message_string ]
     */
    while (running) {
        ssize_t len = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (len <= 0) continue;
        
        /*
         * The buffer may contain multiple netlink messages.
         * We iterate through them using NLMSG_OK and NLMSG_NEXT.
         */
        for (struct nlmsghdr *nlh = (struct nlmsghdr*)buffer; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            /*
             * The payload of an audit message is typically a string.
             * We extract it and print it to the console.
             */
            char *msg_data = (char*)NLMSG_DATA(nlh);
            int msg_len = NLMSG_PAYLOAD(nlh, 0);
            
            if (msg_len > 0) {
                printf("AUDIT_%s: ", get_audit_type(nlh->nlmsg_type));
                
                char *audit_msg = malloc(msg_len + 1);
                if (audit_msg) {
                    memcpy(audit_msg, msg_data, msg_len);
                    audit_msg[msg_len] = '\0';
                    printf("%s\n", audit_msg);
                    free(audit_msg);
                }
            }
        }
    }
    
    close(sock_fd);
    return 0;
}
