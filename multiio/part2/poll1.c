/*
 * 
 *
 * Uses poll() for I/O multiplexing with a timeout.
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
	pollarray[0].events = POLLIN; /* Watch for input */

	pollarray[1].fd = fd2;
	pollarray[1].events = POLLIN; /* Watch for input */

	printf("Waiting for data from pipes (timeout: 10 seconds)...\n");

	/* Wait for events on the file descriptors */
	n = poll(pollarray, NUM_FDS, 10000); /* 10-second timeout */

	if (n == -1) {
		perror("poll");
		close(fd1);
		close(fd2);
		exit(EXIT_FAILURE);
	} else if (n == 0) {
		printf("poll() timed out. No data received.\n");
	} else {
		printf("poll() returned %d. Events detected.\n", n);

		/* Check the revents field for each file descriptor */
		printf("fd1 revents = 0x%x\n", pollarray[0].revents);
		printf("fd2 revents = 0x%x\n", pollarray[1].revents);

		/* You can now check for specific events like POLLIN, POLLHUP, etc. */
		if (pollarray[0].revents & POLLIN) {
			printf("Data is available to read on pone.\n");
		}
		if (pollarray[1].revents & POLLIN) {
			printf("Data is available to read on ptwo.\n");	
		}
	}

	/* Close the file descriptors */
	close(fd1);
	close(fd2);

	return 0;
}

	

