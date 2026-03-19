#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Define a structure that could represent complex thread-specific data
typedef struct {
    int session_id;
    int data;
} session_data;

// Thread-specific key to hold a pointer to session_data
pthread_key_t session_key;

// Cleanup function for thread-specific data
void cleanup_session_data(void *data) {
    if (data != NULL) {
        printf("Cleaning up session data for Thread %ld\n", (long)pthread_self());
        free(data);
    }
}

// Initialize thread-specific key
void create_session_key() {
    pthread_key_create(&session_key, cleanup_session_data);
}

// Function that each thread will run
void* thread_function(void* arg) {
    session_data *my_data;

    // Once initialization to ensure the key is created before any thread uses it
    pthread_once_t key_once = PTHREAD_ONCE_INIT;
    pthread_once(&key_once, create_session_key);

    // Allocate and set thread-specific data
    my_data = malloc(sizeof(session_data));
    if (my_data == NULL) {
        perror("Malloc failed");
        pthread_exit(NULL);
    }
    my_data->session_id = (int)(long)arg; // Just an example assignment
    my_data->data = 0;

    pthread_setspecific(session_key, my_data);

    for (int i = 0; i < 5; i++) {
        my_data->data++;
        printf("Thread %ld, Session %d, Data %d\n", (long)pthread_self(), my_data->session_id, my_data->data);
        sleep(1); // Simulate work
    }

    // Cleanup is handled by the key's destructor function
    return NULL;
}

int main() {
    pthread_t threads[3];

    // Create multiple threads
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, (void*)(long)(i + 1)) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup the thread-specific key
    pthread_key_delete(session_key);

    return 0;
}

