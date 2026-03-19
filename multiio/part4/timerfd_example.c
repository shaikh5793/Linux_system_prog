/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Simple timerfd Example
 * Shows how to receive timer expirations as file descriptor events using timerfd with epoll.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <time.h>
#include <errno.h>

#define MAX_EVENTS 1

int main()
{
	int tfd;
	int epfd;
	struct epoll_event ev, events[MAX_EVENTS];
	struct itimerspec new_value;
	uint64_t expirations;
	ssize_t s;

	/* Create a timerfd file descriptor */
	tfd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (tfd == -1) {
		perror("timerfd_create");
		exit(EXIT_FAILURE);
	}

	printf("=== Techveda TimerFD Demo ===\n");
	printf("Author: Raghu Bharadwaj | www.techveda.org\n");
	printf("timerfd created: %d\n", tfd);

	/* Set the timer to expire after 2 seconds, and then every 1 second */
	new_value.it_value.tv_sec = 2;
	new_value.it_value.tv_nsec = 0;
	new_value.it_interval.tv_sec = 1;
	new_value.it_interval.tv_nsec = 0;

	if (timerfd_settime(tfd, 0, &new_value, NULL) == -1) {
		perror("timerfd_settime");
		exit(EXIT_FAILURE);
	}

	/* Create an epoll instance */
	epfd = epoll_create1(0);
	if (epfd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	/* Add the timerfd to the epoll instance */
	ev.events = EPOLLIN;
	ev.data.fd = tfd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, tfd, &ev) == -1) {
		perror("epoll_ctl: timerfd");
		exit(EXIT_FAILURE);
	}

	printf("Timer set: initial expiration in 2s, then every 1s. Waiting for events...\n");

	while (1) {
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == tfd) {
				s = read(tfd, &expirations, sizeof(uint64_t));
				if (s != sizeof(uint64_t)) {
					perror("read timerfd");
					exit(EXIT_FAILURE);
				}
				printf("Timer expired %lu times.\n", expirations);
			}
		}
	}

	close(tfd);
	close(epfd);

	return 0;
}