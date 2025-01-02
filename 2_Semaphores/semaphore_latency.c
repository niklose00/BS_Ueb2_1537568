#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_ITERATIONS 1000

// Define the semaphore
sem_t semaphore;

// Shared data
volatile int shared_data = 0;

// Timing function
long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

// Sender thread function
void *sender(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        shared_data = i; // Simulate data communication
        sem_post(&semaphore); // Signal the semaphore
    }
    return NULL;
}

// Receiver thread function
void *receiver(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sem_wait(&semaphore); // Wait for the semaphore
        int data = shared_data; // Read shared data
    }
    return NULL;
}

int main() {
    pthread_t sender_thread, receiver_thread;
    struct timespec start, end;

    // Initialize the semaphore
    sem_init(&semaphore, 0, 0);

    // Start timing
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads
    pthread_create(&sender_thread, NULL, sender, NULL);
    pthread_create(&receiver_thread, NULL, receiver, NULL);

    // Wait for threads to finish
    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    // End timing
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time
    long elapsed_time = calculate_elapsed_time(start, end);

    // Calculate average latency
    double average_latency = (double)elapsed_time / (NUM_ITERATIONS * 2);

    // Output results
    printf("Total time: %ld ns\n", elapsed_time);
    printf("Average latency per message: %.2f ns\n", average_latency);

    // Destroy the semaphore
    sem_destroy(&semaphore);

    return 0;
}
