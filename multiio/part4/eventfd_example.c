/*
 * 
 *
 * Simple eventfd Example
 * Shows how to use eventfd for simple event notifications between threads/processes with epoll.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_EVENTS 1

int main()
{
	int efd;
	int epfd;
	struct epoll_event ev, events[MAX_EVENTS];
	uint64_t value;
	ssize_t s;

	/* Create an eventfd file descriptor */
	efd = eventfd(0, 0); /* Initial value 0, no flags */
	if (efd == -1) {
		perror("eventfd");
		exit(EXIT_FAILURE);
	}

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

	printf("Waiting for eventfd to be signaled...\n");
	printf("From another terminal, run: echo 1 > /proc/%d/fd/%d\n", getpid(), efd);
	printf("Or use the eventfd directly with a program that writes to fd %d\n", efd);

	while (1) {
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == efd) {
				s = read(efd, &value, sizeof(uint64_t));
				if (s != sizeof(uint64_t)) {
					perror("read eventfd");
					exit(EXIT_FAILURE);
				}
				printf("Event received! Value: %lu\n", value);
				printf("Waiting for eventfd to be signaled again...\n");
			}
		}
	}

	close(efd);
	close(epfd);

	return 0;
}