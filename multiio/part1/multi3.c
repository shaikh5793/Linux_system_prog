/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Uses pselect() for I/O multiplexing with signal handling.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

int main()
{
	int fd1, fd2;
	fd_set read_set;
	char buf[10];
	struct timespec timeout;
	sigset_t mask;
	int n;

	/* Open the first named pipe (FIFO) */
	fd1 = open("./pone", O_RDWR | O_NONBLOCK);
	if (fd1 == -1) {
		perror("open pone");
		exit(EXIT_FAILURE);
	}

	/* Open the second named pipe (FIFO) */
	fd2 = open("./ptwo", O_RDWR | O_NONBLOCK);
	if (fd2 == -1) {
		perror("open ptwo");
		close(fd1);
		exit(EXIT_FAILURE);
	}

	/* Block SIGINT and SIGQUIT signals */
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
		perror("sigprocmask");
		close(fd1);
		close(fd2);
		exit(EXIT_FAILURE);
	}

	printf("Waiting for data... (SIGINT and SIGQUIT are blocked during pselect)\n");

	while (1) {
		/* Set a 10-second timeout for pselect() */
		timeout.tv_sec = 10;
		timeout.tv_nsec = 0;

		/* Clear the read_set and add the file descriptors to it */
		FD_ZERO(&read_set);
		FD_SET(fd1, &read_set);
		FD_SET(fd2, &read_set);

		/* Determine the highest file descriptor for pselect() */
		int max_fd = max(fd1, fd2) + 1;

		/* Atomically unblock signals and wait for I/O or a signal */
		n = pselect(max_fd, &read_set, NULL, NULL, &timeout, &mask);

		if (n == -1) {
			if (errno == EINTR) {
				printf("pselect was interrupted by a signal.\n");
				continue; /* Restart the loop */
			}
			perror("pselect");
			break; /* Exit the loop on other errors */
		} else if (n == 0) {
			printf("pselect() timed out. No data received.\n");
		} else {
			/* Check if fd1 is ready for reading */
			if (FD_ISSET(fd1, &read_set)) {
				printf("Reading from fd1 (pone)\n");
				n = read(fd1, buf, 10);
				if (n > 0) {
					printf("Read %d bytes from pone\n", n);
				} else if (n == 0) {
					printf("End of file on pone\n");
				} else {
					perror("read pone");
				}
			}

			/* Check if fd2 is ready for reading */
			if (FD_ISSET(fd2, &read_set)) {
				printf("Reading from fd2 (ptwo)\n");
				n = read(fd2, buf, 10);
				if (n > 0) {
					printf("Read %d bytes from ptwo\n", n);
				} else if (n == 0) {
					printf("End of file on ptwo\n");
				} else {
					perror("read ptwo");
				}
			}
		}
	}

	/* Close the file descriptors */
	close(fd1);
	close(fd2);

	return 0;
}
