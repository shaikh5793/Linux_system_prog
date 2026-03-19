/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Basic blocking I/O on multiple file descriptors.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int main()
{
	int fd1, fd2;
	char buf[10];
	int n;

	/* Open the first named pipe (FIFO) */
	/* The program will block here until the other end of the pipe is opened for writing */
	fd1 = open("./pone", O_RDWR);
	if (fd1 == -1) {
		perror("open pone");
		exit(EXIT_FAILURE);
	}

	/* Open the second named pipe (FIFO) */
	fd2 = open("./ptwo", O_RDWR);
	if (fd2 == -1) {
		perror("open ptwo");
		close(fd1); /* Close the first file descriptor before exiting */
		exit(EXIT_FAILURE);
	}

	printf("=== Techveda Multi-I/O Demo ===\n");
	printf("Trying to read from pone\n");
	/* Read from the first pipe. This is a blocking call. */
	/* The program will wait here until there is data to be read from the pipe. */
	n = read(fd1, buf, 10);
	if (n == -1) {
		perror("read pone");
		close(fd1);
		close(fd2);
		exit(EXIT_FAILURE);
	}
	printf("Read %d no of bytes from pone\n", n);

	printf("Trying to read from ptwo\n");
	/* Read from the second pipe. This is also a blocking call. */
	n = read(fd2, buf, 10);
	if (n == -1) {
		perror("read ptwo");
		close(fd1);
		close(fd2);
		exit(EXIT_FAILURE);
	}
	printf("Read %d no of bytes from ptwo\n", n);

	/* Close the file descriptors */
	close(fd1);
	close(fd2);

	return 0;
}

