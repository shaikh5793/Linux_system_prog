/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Simple EPOLLOUT Example
 * Demonstrates how to detect write readiness on a non-blocking file descriptor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_EVENTS 1
#define BUF_SIZE 4096 /* A size that might fill a pipe buffer quickly */

int main()
{
	int pipefd[2];
	int epfd;
	struct epoll_event ev, events[MAX_EVENTS];
	char buffer[BUF_SIZE] = {0}; /* Initialize buffer */
	ssize_t bytes_written = 0;
	int total_written = 0;

	/* Create a pipe */
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	/* Set the write end of the pipe to non-blocking */
	if (fcntl(pipefd[1], F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl O_NONBLOCK");
		exit(EXIT_FAILURE);
	}

	/* Create an epoll instance */
	epfd = epoll_create1(0);
	if (epfd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	/* Add the write end of the pipe to the epoll instance, watching for EPOLLOUT */
	ev.events = EPOLLOUT; /* Watch for write readiness */
	ev.data.fd = pipefd[1];
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[1], &ev) == -1) {
		perror("epoll_ctl: pipefd[1]");
		exit(EXIT_FAILURE);
	}

	printf("Attempting to write to pipe (fd %d) until EAGAIN...\n", pipefd[1]);

	/* Fill the pipe buffer until write returns EAGAIN */
	while (1) {
		bytes_written = write(pipefd[1], buffer, BUF_SIZE);
		if (bytes_written == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				printf("Pipe buffer full. write() returned EAGAIN/EWOULDBLOCK. Total written: %d bytes.\n", total_written);
				break;
			} else {
				perror("write");
				exit(EXIT_FAILURE);
			}
		}
		total_written += (int)bytes_written;
		printf("Wrote %zd bytes. Total: %d\n", bytes_written, total_written);
	}

	printf("Waiting for EPOLLOUT event on pipefd[1]...\n");

	/* Wait for EPOLLOUT event */
	int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); /* Infinite timeout */

	if (nfds == -1) {
		perror("epoll_wait");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < nfds; i++) {
		if (events[i].data.fd == pipefd[1] && (events[i].events & EPOLLOUT)) {
			printf("Received EPOLLOUT event on fd %d. Pipe is now writable.\n", events[i].data.fd);
			/* Try writing again */
			bytes_written = write(pipefd[1], buffer, BUF_SIZE);
			if (bytes_written == -1) {
				perror("write after EPOLLOUT");
			} else {
				printf("Successfully wrote %zd bytes after EPOLLOUT.\n", bytes_written);
			}
		}
	}

	/* Clean up */
	close(pipefd[0]);
	close(pipefd[1]);
	close(epfd);

	return 0;
}