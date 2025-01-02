#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define NUM_ITERATIONS 1000

// Define the Spinlock
volatile atomic_flag spinlock = ATOMIC_FLAG_INIT;

// Shared data
volatile int shared_data = 0;

// Timing function
long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

// Lock and unlock functions
void lock(volatile atomic_flag *lock) {
    while (atomic_flag_test_and_set(lock)) {
        // Busy waiting
    }
}

void unlock(volatile atomic_flag *lock) {
    atomic_flag_clear(lock);
}

// Thread functions
void *sender(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        lock(&spinlock);
        shared_data = i; // Simulate data communication
        unlock(&spinlock);
    }
    return NULL;
}

void *receiver(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        while (atomic_flag_test_and_set(&spinlock)) {
            // Busy waiting
        }
        int data = shared_data; // Read shared data
        unlock(&spinlock);
    }
    return NULL;
}

int main() {
    pthread_t sender_thread, receiver_thread;
    struct timespec start, end;

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

    return 0;
}
