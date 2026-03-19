#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Declare a thread-local variable
__thread int counter = 0;

// Function that each thread will run
void* thread_function(void* arg) {
    for (int i = 0; i < 5; i++) {
        counter++;
        printf("Thread %ld, Counter Value: %d %p\n", (long)pthread_self(), counter, &counter);
        sleep(1); // Simulate work
    }
    return NULL;
}

int main() {
    pthread_t threads[3];

    // Create multiple threads
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

