/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Simple signalfd Example
 * Demonstrates how to receive signals as file descriptor events using signalfd with epoll.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_EVENTS 1

int main()
{
	sigset_t mask;
	int sfd;
	int epfd;
	struct epoll_event ev, events[MAX_EVENTS];
	struct signalfd_siginfo fdsi;
	ssize_t s;

	/* Block signals so that they are not handled by the default handler */
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);

	/* Block the signals in the process's signal mask */
	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	/* Create a signalfd file descriptor */
	sfd = signalfd(-1, &mask, 0);
	if (sfd == -1) {
		perror("signalfd");
		exit(EXIT_FAILURE);
	}

	printf("signalfd created: %d\n", sfd);

	/* Create an epoll instance */
	epfd = epoll_create1(0);
	if (epfd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	/* Add the signalfd to the epoll instance */
	ev.events = EPOLLIN;
	ev.data.fd = sfd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev) == -1) {
		perror("epoll_ctl: signalfd");
		exit(EXIT_FAILURE);
	}

	printf("Waiting for SIGINT or SIGTERM (try Ctrl+C or 'kill %d')...\n", getpid());

	while (1) {
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == sfd) {
				s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
				if (s != sizeof(struct signalfd_siginfo)) {
					perror("read signalfd");
					exit(EXIT_FAILURE);
				}

				if (fdsi.ssi_signo == SIGINT) {
					printf("Caught SIGINT! Exiting.\n");
					goto end;
				} else if (fdsi.ssi_signo == SIGTERM) {
					printf("Caught SIGTERM! Exiting.\n");
					goto end;
				} else {
					printf("Caught unexpected signal %d\n", fdsi.ssi_signo);
				}
			}
		}
	}

end:
	close(sfd);
	close(epfd);

	return 0;
}