/*
 * Copyright (c) 2024 Techveda
 * Author: Raghu Bharadwaj
 * SPDX-License-Identifier: MIT
 *
 * Website: www.techveda.org
 * Email: raghu@techveda.org
 *
 * Simple test program to signal an eventfd
 * Usage: ./test_eventfd <pid> <fd> <value>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <pid> <fd> <value>\n", argv[0]);
        fprintf(stderr, "Example: %s 1234 3 5\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pid = atoi(argv[1]);
    int fd_num = atoi(argv[2]);
    uint64_t value = (uint64_t)atoi(argv[3]);

    char fd_path[256];
    snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%d", pid, fd_num);

    int fd = open(fd_path, O_WRONLY);
    if (fd == -1) {
        perror("open eventfd");
        return EXIT_FAILURE;
    }

    ssize_t written = write(fd, &value, sizeof(value));
    if (written != sizeof(value)) {
        perror("write to eventfd");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Successfully wrote value %lu to eventfd at %s\n", value, fd_path);
    close(fd);
    return EXIT_SUCCESS;
}
