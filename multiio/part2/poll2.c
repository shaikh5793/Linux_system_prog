/*
 * 
 *
 * Uses poll() for I/O multiplexing with an infinite timeout.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#define NUM_FDS 2

int main()
{
	int fd1, fd2;
	struct pollfd pollarray[NUM_FDS];
	char buf[10];
	int n;

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

	/* Initialize the pollfd structure */
	pollarray[0].fd = fd1;
	pollarray[0].events = POLLIN;

	pollarray[1].fd = fd2;
	pollarray[1].events = POLLIN;

	printf("Waiting for data from pipes...\n");

	while (1) {
		/* Wait for events on the file descriptors (infinite timeout) */
		n = poll(pollarray, NUM_FDS, -1);

		if (n == -1) {
			perror("poll");
			break; /* Exit loop on error */
		}

		/* Check if fd1 has data to read */
		if (pollarray[0].revents & POLLIN) {
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

		/* Check if fd2 has data to read */
		if (pollarray[1].revents & POLLIN) {
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

	/* Close the file descriptors */
	close(fd1);
	close(fd2);

	return 0;
}

