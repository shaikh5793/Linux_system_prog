/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Demonstrates the epoll API for efficient I/O multiplexing (basic EPOLLIN).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_EVENTS 2

int main()
{
	int fd1, fd2, epfd, ready;
	struct epoll_event ev, events[MAX_EVENTS];
	char buf[10];
	ssize_t n;
	int i;

	/* Open the first named pipe (FIFO) in non-blocking mode */
	fd1 = open("./pone", O_RDWR | O_NONBLOCK);
	if (fd1 == -1) {
		perror("open pone");
		exit(EXIT_FAILURE);
	}

	/* Open the second named pipe (FIFO) in non-blocking mode */
	fd2 = open("./ptwo", O_RDWR | O_NONBLOCK);
	if (fd2 == -1) {
		perror("open ptwo");
		close(fd1);
		exit(EXIT_FAILURE);
	}

	/* Create an epoll instance */
	epfd = epoll_create1(0);
	if (epfd == -1) {
		perror("epoll_create1");
		close(fd1);
		close(fd2);
		exit(EXIT_FAILURE);
	}

	printf("=== Techveda Linux Multi-I/O Examples ===\n");
	printf("fd1 is %d & fd2 is %d\n", fd1, fd2);

	/* Add fd1 to the epoll set (edge-triggered) */
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = fd1;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &ev) == -1) {
		perror("epoll_ctl: fd1");
		close(fd1);
		close(fd2);
		close(epfd);
		exit(EXIT_FAILURE);
	}

	/* Add fd2 to the epoll set (level-triggered) */
	ev.events = EPOLLIN;
	ev.data.fd = fd2;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd2, &ev) == -1) {
		perror("epoll_ctl: fd2");
		close(fd1);
		close(fd2);
		close(epfd);
		exit(EXIT_FAILURE);
	}

	printf("Waiting for data from pipes...\n");

	while (1) {
		/* Wait for events */
		ready = epoll_wait(epfd, events, MAX_EVENTS, -1);

		if (ready == -1) {
			perror("epoll_wait");
			break; /* Exit loop on error */
		}

		printf("Ready: %d\n", ready);

		/* Process events */
		for (i = 0; i < ready; i++) {
			if (events[i].events & EPOLLIN) {
				printf("File descriptor %d is ready for reading.\n", events[i].data.fd);
				n = read(events[i].data.fd, buf, 10);
				if (n > 0) {
					printf("Read %zd bytes from fd %d\n", n, events[i].data.fd);
				} else if (n == 0) {
					printf("End of file on fd %d\n", events[i].data.fd);
				} else {
					/* For edge-triggered, we might get EAGAIN if we read all data */
					if (errno != EAGAIN) {
						perror("read");
					}
				}
			}
		}
	}

	/* Clean up */
	close(fd1);
	close(fd2);
	close(epfd);

	return 0;
}
