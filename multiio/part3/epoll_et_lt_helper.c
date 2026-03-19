/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Helper program for epoll Edge-Triggered vs. Level-Triggered demonstration.
 */

/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_EVENTS 1
#define BUF_SIZE 10

int main(int argc, char *argv[])
{
	int pipefd[2];
	int epfd;
	struct epoll_event ev, events[MAX_EVENTS];
	char buf[BUF_SIZE];
	ssize_t bytes_read;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <lt|et>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Create a pipe */
	if (pipe(pipefd) == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	/* Set the read end of the pipe to non-blocking */
	if (fcntl(pipefd[0], F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl O_NONBLOCK");
		return EXIT_FAILURE;
	}

	/* Create an epoll instance */
	epfd = epoll_create1(0);
	if (epfd == -1) {
		perror("epoll_create1");
		return EXIT_FAILURE;
	}

	/* Add the read end of the pipe to the epoll instance */
	ev.events = EPOLLIN;
	if (strcmp(argv[1], "et") == 0) {
		ev.events |= EPOLLET; /* Edge-triggered */
		printf("Monitoring pipe (fd %d) with Edge-Triggered epoll.\n", pipefd[0]);
	} else {
		/* Default to Level-triggered */
		printf("Monitoring pipe (fd %d) with Level-Triggered epoll.\n", pipefd[0]);
	}
	ev.data.fd = pipefd[0];

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[0], &ev) == -1) {
		perror("epoll_ctl: pipefd[0]");
		return EXIT_FAILURE;
	}

	printf("Pipe write end fd: %d\n", pipefd[1]);
	printf("You can write to this pipe from another terminal, e.g., 'echo hello > /proc/self/fd/%d'\n", pipefd[1]);
	printf("Or use the provided test script.\n");

	while (1) {
		printf("\nWaiting for epoll event...\n");
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);

		if (nfds == -1) {
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == pipefd[0] && (events[i].events & EPOLLIN)) {
				printf("EPOLLIN event received on fd %d.\n", pipefd[0]);
				/* Read all available data for ET, or just some for LT demo */
				while ((bytes_read = read(pipefd[0], buf, BUF_SIZE)) > 0) {
					printf("  Read %zd bytes: \"%.*s\"\n", bytes_read, (int)bytes_read, buf);
				}
				if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
					printf("  No more data to read (EAGAIN/EWOULDBLOCK).\n");
				} else if (bytes_read == 0) {
					printf("  End of file on pipe.\n");
					goto end;
				} else if (bytes_read == -1) {
					perror("read");
					goto end;
				}
			}
		}
	}

end:
	close(pipefd[0]);
	close(pipefd[1]);
	close(epfd);

	return 0;
}