#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STACK_SIZE 1024 * 1024  // 1 MB stack

int child_func(void *arg) {
    printf("Child: PID = %d, PPID = %d\n", getpid(), getppid());

    // Modify shared variable
    int *shared = (int *)arg;
    (*shared)++;

    printf("Child: shared value = %d\n", *shared);
    return 0;
}

int main() {
    void *stack;
    void *stack_top;
    int shared = 100;

    // Allocate stack
    stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    stack_top = (char *)stack + STACK_SIZE;

    int flags = CLONE_VM | CLONE_FS | CLONE_FILES |
                CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM;

    int pid = clone(child_func, stack_top, flags, &shared);
    if (pid == -1) {
        perror("clone");
        exit(1);
    }

    // Give child time to run
    sleep(1);

    printf("Parent: shared value = %d\n", shared);

    free(stack);
    return 0;
}
