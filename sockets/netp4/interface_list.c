/* Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Lists all network interfaces using the NETLINK_ROUTE family of netlink sockets.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>

int main() {
    int sock_fd;
    struct sockaddr_nl addr;
    char buffer[4096];
    
    printf("Simple Interface List (NETLINK_ROUTE)\n");
    printf("====================================\n");
    
    /*
     * Step 1: Create the netlink socket.
     * This is the first step in any netlink communication.
     * We use AF_NETLINK as the address family.
     * SOCK_RAW is used for NETLINK_ROUTE.
     * NETLINK_ROUTE is the protocol for routing and link configuration.
     */
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock_fd == -1) {
        perror("socket");
        exit(1);
    }
    
    /*
     * Step 2: Bind the socket to a local address.
     * The address includes our process ID (PID) for the kernel to identify us.
     * We set nl_groups to 0 because we are not listening to any multicast groups.
     */
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 0;
    
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sock_fd);
        exit(1);
    }
    
    /*
     * Step 3: Construct the netlink request message.
     * We are requesting a list of all network links (RTM_GETLINK).
     * The NLM_F_DUMP flag asks for a complete list.
     * The message contains a standard netlink message header (nlmsghdr)
     * and a payload specific to the request type (ifinfomsg).
     *
     * Packet Layout: [ nlmsghdr | ifinfomsg ]
     */
    struct {
        struct nlmsghdr nlh;
        struct ifinfomsg ifm;
    } req;
    
    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.nlh.nlmsg_type = RTM_GETLINK;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nlh.nlmsg_seq = 1;
    req.nlh.nlmsg_pid = getpid();
    req.ifm.ifi_family = AF_UNSPEC;
    
    /*
     * Step 4: Send the request to the kernel.
     * The destination address has nl_pid set to 0, which represents the kernel.
     */
    struct sockaddr_nl dest;
    memset(&dest, 0, sizeof(dest));
    dest.nl_family = AF_NETLINK;
    dest.nl_pid = 0;
    
    if (sendto(sock_fd, &req, sizeof(req), 0, (struct sockaddr*)&dest, sizeof(dest)) == -1) {
        perror("sendto");
        close(sock_fd);
        exit(1);
    }
    
    /*
     * Step 5: Receive and process the kernel's response.
     * The response may come in multiple parts.
     * We loop until we receive a NLMSG_DONE message, indicating the end of the list.
     */
    printf("Network Interfaces:\n");
    
    while (1) {
        ssize_t len = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (len <= 0) break;
        
        /*
         * The buffer may contain multiple netlink messages.
         * We use the NLMSG_OK and NLMSG_NEXT macros to iterate through them.
         *
         * Response Packet Layout: [ nlmsghdr | ifinfomsg | rtattr | ... ]
         */
        for (struct nlmsghdr *nlh = (struct nlmsghdr*)buffer; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_type == NLMSG_DONE) {
                printf("\nDone.\n");
                close(sock_fd);
                return 0;
            }
            
            if (nlh->nlmsg_type == RTM_NEWLINK) {
                /*
                 * For each RTM_NEWLINK message, we extract the interface information.
                 * The interface name is stored in the message attributes.
                 * We parse these attributes to find the IFLA_IFNAME attribute.
                 */
                struct ifinfomsg *ifm = (struct ifinfomsg*)NLMSG_DATA(nlh);
                char ifname[IFNAMSIZ] = "unknown";
                
                struct rtattr *rta = IFLA_RTA(ifm);
                int rta_len = IFLA_PAYLOAD(nlh);
                
                for (; RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
                    if (rta->rta_type == IFLA_IFNAME) {
                        strncpy(ifname, (char*)RTA_DATA(rta), IFNAMSIZ - 1);
                        break;
                    }
                }
                
                printf("  %s (index: %d, %s)\n", 
                       ifname, ifm->ifi_index,
                       (ifm->ifi_flags & IFF_UP) ? "UP" : "DOWN");
            }
        }
    }
    
    close(sock_fd);
    return 0;
}
