#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>

pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;

// Cleanup handlers
void file_cleanup_handler(void *arg) {
    int fd = *(int *)arg;  
    close(fd);
    free(arg); // Free memory storing the file descriptor
    printf("%s File closed.\n",__func__); 
}

void memory_cleanup_handler(void *arg) {
    free(arg);
    printf("%s Memory freed.\n",__func__);
}

void mutex_cleanup_handler(void *arg) {
    pthread_mutex_unlock((pthread_mutex_t *)arg);
    printf("%s Mutex unlocked.\n",__func__);
}

void *thread_func(void *arg) {
    // Stage 1: Open a file
    int fd = open("workfile.txt", O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        fprintf(stderr, "Error opening file\n");
        return NULL;  
    } 

    int *fd_ptr = malloc(sizeof(int));
    *fd_ptr = fd;
    pthread_cleanup_push(file_cleanup_handler, fd_ptr);

    // Stage 2: Allocate memory
    char *buffer = malloc(1024); 
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL; 
    }
    pthread_cleanup_push(memory_cleanup_handler, buffer);

    // Stage 3: Lock a mutex
    pthread_mutex_lock(&work_mutex);
    pthread_cleanup_push(mutex_cleanup_handler, &work_mutex); 

    // ... Work with resources: Write, Read, more mutex ops ...
    strcpy(buffer, "Sample data to write to the file\n"); 
    write(fd, buffer, strlen(buffer)); 

    sleep(3);
    // Optional: Simulate an error after writing to the file
    // if (error_condition) {
    //    pthread_exit(NULL); // Abrupt thread termination
    // }

    printf("%s: invoking pop routines\n",__func__);
    pthread_cleanup_pop(1); // Unlock mutex
    pthread_cleanup_pop(1); // Free memory
    pthread_cleanup_pop(1); // Close file
    
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    sleep(2);

    pthread_cancel(thread);
    pthread_join(thread, NULL); 
    return 0;
}

