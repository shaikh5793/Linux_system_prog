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

// Mutex initializer
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function executed by thread 1
static void *thread_func1(void *ignored_argument) {
    int s;

    printf("\n%s:\n", __func__);
    s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (s != 0) {
        perror("pthread_setcancelstate");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&mutex);
    printf("Thread 1 acquired mutex\n");
    sleep(10);  // Hold the mutex for 10 seconds
    pthread_mutex_unlock(&mutex);
    printf("Thread 1 released mutex\n");

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    while (1) {
        getchar();  // Keep thread running and waiting for input
    }

    return NULL;
}

// Function executed by thread 2
static void *thread_func2(void *ignored_argument) {
    printf("\n%s:\n", __func__);
    sleep(5);  // Wait before attempting to acquire the mutex

    pthread_mutex_lock(&mutex);
    printf("Thread 2 acquired mutex\n");
    pthread_mutex_unlock(&mutex);
    printf("Thread 2 released mutex\n");

    return NULL;
}

int main(void) {
    pthread_t thr1, thr2;
    void *res;
    int s;

    // Create thread 1
    s = pthread_create(&thr1, NULL, thread_func1, NULL);
    if (s != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Create thread 2
    s = pthread_create(&thr2, NULL, thread_func2, NULL);
    if (s != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    sleep(2);  // Give threads a chance to start

    printf("main(): sending cancellation request\n");
    s = pthread_cancel(thr1);
    if (s != 0) {
        perror("pthread_cancel");
        exit(EXIT_FAILURE);
    }

    // Join with thread 1 and check its exit status
    s = pthread_join(thr1, &res);
    if (s != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (res == PTHREAD_CANCELED)
        printf("main(): thread was canceled\n");
    else
        printf("main(): thread wasn't canceled\n");

    // Join with thread 2
    s = pthread_join(thr2, &res);
    if (s != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    return 0;
}

