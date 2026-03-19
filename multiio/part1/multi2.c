/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Uses select() for I/O multiplexing with a timeout.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

int main()
{
	int fd1, fd2;
	fd_set read_set;
	char buf[10];
	struct timeval timeout;
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

	/* Set a 5-second timeout for select() */
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	/* Clear the read_set and add the file descriptors to it */
	FD_ZERO(&read_set);
	FD_SET(fd1, &read_set);
	FD_SET(fd2, &read_set);

	/* Determine the highest file descriptor for select() */
	int max_fd = max(fd1, fd2) + 1;

	printf("Waiting for data from pipes (timeout: 5 seconds)...\n");

	/* Wait for data to become available on either of the file descriptors */
	n = select(max_fd, &read_set, NULL, NULL, &timeout);

	if (n == -1) {
		perror("select");
		close(fd1);
		close(fd2);
		exit(EXIT_FAILURE);
	} else if (n == 0) {
		printf("select() timed out. No data received.\n");
	} else {
		printf("Time left %ld seconds\n", timeout.tv_sec);

		/* Check if fd1 is ready for reading */
		if (FD_ISSET(fd1, &read_set)) {
			printf("Reading from fd1 (pone)\n");
			n = read(fd1, buf, 10);
			if (n == -1) {
				perror("read pone");
			} else {
				printf("Read %d bytes from pone\n", n);
			}
		}

		/* Check if fd2 is ready for reading */
		if (FD_ISSET(fd2, &read_set)) {
			printf("Reading from fd2 (ptwo)\n");
				n = read(fd2, buf, 10);
				if (n == -1) {
					perror("read ptwo");
				} else {
					printf("Read %d bytes from ptwo\n", n);
				}
		}
	}

	/* Close the file descriptors */
	close(fd1);
	close(fd2);

	return 0;
}

