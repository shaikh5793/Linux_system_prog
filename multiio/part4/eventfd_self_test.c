/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Self-contained eventfd Example
 * Demonstrates eventfd functionality using fork() to create child process
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdint.h>

#define MAX_EVENTS 1

int main()
{
    int efd;
    int epfd;
    struct epoll_event ev, events[MAX_EVENTS];
    uint64_t value;
    pid_t child_pid;
    ssize_t s;

    /* Create an eventfd file descriptor */
    efd = eventfd(0, 0); /* Initial value 0, no flags */
    if (efd == -1) {
        perror("eventfd");
        exit(EXIT_FAILURE);
    }

	printf("=== Techveda EventFD Demo ===\n");
	printf("Author: Raghu Bharadwaj | www.techveda.org\n");
	printf("eventfd created: %d\n", efd);

    /* Create an epoll instance */
    epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    /* Add the eventfd to the epoll instance */
    ev.events = EPOLLIN; /* Watch for read readiness */
    ev.data.fd = efd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, efd, &ev) == -1) {
        perror("epoll_ctl: eventfd");
        exit(EXIT_FAILURE);
    }

    /* Fork to create a child process that will signal the eventfd */
    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        /* Child process: signal the eventfd after a delay */
        printf("Child process: sleeping for 2 seconds before signaling eventfd...\n");
        sleep(2);
        
        uint64_t signal_value = 42;
        s = write(efd, &signal_value, sizeof(signal_value));
        if (s != sizeof(signal_value)) {
            perror("child write to eventfd");
            exit(EXIT_FAILURE);
        }
        printf("Child process: signaled eventfd with value %lu\n", signal_value);
        
        /* Signal again after another delay */
        sleep(1);
        signal_value = 123;
        s = write(efd, &signal_value, sizeof(signal_value));
        if (s != sizeof(signal_value)) {
            perror("child second write to eventfd");
            exit(EXIT_FAILURE);
        }
        printf("Child process: signaled eventfd again with value %lu\n", signal_value);
        exit(EXIT_SUCCESS);
    }

    /* Parent process: wait for eventfd events */
    printf("Parent process: waiting for eventfd signals...\n");
    
    int events_received = 0;
    while (events_received < 2) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, 5000); /* 5 second timeout */
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        } else if (nfds == 0) {
            printf("Timeout waiting for eventfd signal\n");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == efd) {
                s = read(efd, &value, sizeof(uint64_t));
                if (s != sizeof(uint64_t)) {
                    perror("read eventfd");
                    exit(EXIT_FAILURE);
                }
                printf("Parent process: Event received! Value: %lu\n", value);
                events_received++;
            }
        }
    }

    /* Wait for child process to complete */
    int status;
    waitpid(child_pid, &status, 0);
    printf("Child process completed with status %d\n", WEXITSTATUS(status));

    close(efd);
    close(epfd);

    printf("eventfd example completed successfully!\n");
    return EXIT_SUCCESS;
}
