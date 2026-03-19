/*
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * ******************************************************************************
 * This program is part of the source code provided with "Linux Systems Programming" Course
 * (c) 2019- 2021 Techveda, Hyderabad, India
 * website : www.techveda.org
 * Author/Trainer:  Raghu Bharadwaj
 * ******************************************************************************
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Cleanup function to be called on thread cancellation
void cleanup(void *p) {
    pthread_mutex_unlock(&mutex);
    printf("Cleanup: mutex released\n");
}

// Thread function that might be canceled
void *thread_func1(void *ignored_argument) {
    // Lock the mutex at the beginning of the thread
    pthread_mutex_lock(&mutex);
    pthread_cleanup_push(cleanup, NULL);  // Register cleanup handler

    printf("Thread 1: mutex acquired\n");
    sleep(10); // Simulate work by sleeping

    // Cleanup handler will take care of unlocking mutex
    pthread_cleanup_pop(1); // Execute cleanup handler on pop

    printf("Thread 1: completing normally\n");
    return NULL;  // Return normally if not canceled
}

int main(void) {
    pthread_t thr1;
    void *res;
    int s;

    // Create thread
    s = pthread_create(&thr1, NULL, thread_func1, NULL);
    if (s != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    sleep(2); // Allow the thread some time to start and acquire the mutex
    printf("\nmain(): sending cancellation request\n");

    // Send a cancellation request to the thread
    s = pthread_cancel(thr1);
    if (s != 0) {
        perror("pthread_cancel");
        exit(EXIT_FAILURE);
    }

    // Join with the thread to check its exit status
    s = pthread_join(thr1, &res);
    if (s != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (res == PTHREAD_CANCELED) {
        printf("main(): thread was canceled\n");
    } else {
        // This message would appear if the thread terminated normally and was not canceled
        printf("main(): thread wasn't canceled\n");
    }

    return 0;
}

