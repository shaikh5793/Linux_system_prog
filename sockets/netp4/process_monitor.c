/* Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Monitors and displays process lifecycle events using the NETLINK_CONNECTOR family.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <linux/cn_proc.h>
#include <signal.h>

static int sock_fd = -1;
static volatile int running = 1;

void cleanup(int sig) {
    (void)sig;
    running = 0;
    if (sock_fd != -1) close(sock_fd);
    printf("\nMonitor stopped.\n");
    exit(0);
}

int main() {
    struct sockaddr_nl addr;
    char buffer[4096];
    
    printf("Simple Process Monitor (NETLINK_CONNECTOR)\n");
    printf("==========================================\n");
    
    if (geteuid() != 0) {
        printf("This program requires root privileges.\n");
        printf("Please run with sudo.\n");
        exit(1);
    }
    
    signal(SIGINT, cleanup);
    
    /*
     * Step 1: Create the netlink socket.
     * We use SOCK_DGRAM for NETLINK_CONNECTOR.
     * NETLINK_CONNECTOR is a generic messaging service for the kernel.
     */
    sock_fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
    if (sock_fd == -1) {
        perror("socket");
        exit(1);
    }
    
    /*
     * Step 2: Bind the socket to a local address.
     * We subscribe to the process events multicast group (CN_IDX_PROC).
     */
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = CN_IDX_PROC;
    
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sock_fd);
        exit(1);
    }
    
    /*
     * Step 3: Send a message to the kernel to start listening for events.
     * We construct a message with a connector message header (cn_msg)
     * and set the operation to PROC_CN_MCAST_LISTEN.
     *
     * Packet Layout: [ nlmsghdr | cn_msg | proc_cn_mcast_op ]
     */
    struct {
        struct nlmsghdr nl_hdr;
        struct cn_msg cn_msg;
        enum proc_cn_mcast_op cn_mcast;
    } req;
    
    memset(&req, 0, sizeof(req));
    req.nl_hdr.nlmsg_len = sizeof(req);
    req.nl_hdr.nlmsg_pid = getpid();
    req.nl_hdr.nlmsg_type = NLMSG_DONE;
    
    req.cn_msg.id.idx = CN_IDX_PROC;
    req.cn_msg.id.val = CN_VAL_PROC;
    req.cn_msg.len = sizeof(enum proc_cn_mcast_op);
    req.cn_mcast = PROC_CN_MCAST_LISTEN;
    
    if (send(sock_fd, &req, sizeof(req), 0) == -1) {
        perror("send");
        close(sock_fd);
        exit(1);
    }
    
    printf("Monitoring process events... (Press Ctrl+C to stop)\n\n");
    
    /*
     * Step 4: Continuously receive and process events from the kernel.
     * The kernel sends events as netlink messages.
     * We extract the process event data and print it.
     *
     * Packet Layout: [ nlmsghdr | cn_msg | proc_event ]
     */
    while (running) {
        ssize_t len = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (len <= 0) continue;
        
        struct nlmsghdr *nlh = (struct nlmsghdr*)buffer;
        if (nlh->nlmsg_type == NLMSG_DONE) {
            struct cn_msg *cn_hdr = (struct cn_msg*)NLMSG_DATA(nlh);
            if (cn_hdr->id.idx == CN_IDX_PROC && cn_hdr->id.val == CN_VAL_PROC) {
                struct proc_event *proc_ev = (struct proc_event*)cn_hdr->data;
                
                switch (proc_ev->what) {
                    case PROC_EVENT_FORK:
                        printf("FORK: Parent PID %d -> Child PID %d\n",
                               proc_ev->event_data.fork.parent_pid,
                               proc_ev->event_data.fork.child_pid);
                        break;
                    case PROC_EVENT_EXEC:
                        printf("EXEC: PID %d\n",
                               proc_ev->event_data.exec.process_pid);
                        break;
                    case PROC_EVENT_EXIT:
                        printf("EXIT: PID %d (code: %d)\n",
                               proc_ev->event_data.exit.process_pid,
                               proc_ev->event_data.exit.exit_code);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    close(sock_fd);
    return 0;
}
