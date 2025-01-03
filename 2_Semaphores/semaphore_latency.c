#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>

#define NUM_ITERATIONS 1000

sem_t semaphore;

// Timing function
long calculate_elapsed_time(struct timespec start, struct timespec end) {
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    if (nanoseconds < 0) {
        seconds -= 1;
        nanoseconds += 1e9;
    }
    return seconds * 1e9 + nanoseconds;
}

// Sender thread function
void *sender(void *arg) {
    struct timespec *timing = (struct timespec *)arg;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &timing[i * 2]); // Startzeit
        sem_post(&semaphore); // Signalisiere dem Empfänger
    }
    return NULL;
}

// Receiver thread function
void *receiver(void *arg) {
    struct timespec *timing = (struct timespec *)arg;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sem_wait(&semaphore); // Warten auf Signal
        clock_gettime(CLOCK_MONOTONIC_RAW, &timing[i * 2 + 1]); // Endzeit
    }
    return NULL;
}

int main() {
    pthread_t sender_thread, receiver_thread;
    struct timespec timing[NUM_ITERATIONS * 2];
    long latencies[NUM_ITERATIONS];

    // Initialize semaphore
    sem_init(&semaphore, 0, 0);

    // Create threads
    pthread_create(&sender_thread, NULL, sender, timing);
    pthread_create(&receiver_thread, NULL, receiver, timing);

    // Wait for threads to finish
    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    // Calculate latencies
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        latencies[i] = calculate_elapsed_time(timing[i * 2], timing[i * 2 + 1]);
    }

    // Calculate statistics
    long total_latency = 0;
    long min_latency = latencies[0];
    long max_latency = latencies[0];
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        total_latency += latencies[i];
        if (latencies[i] < min_latency) min_latency = latencies[i];
        if (latencies[i] > max_latency) max_latency = latencies[i];
    }
    double mean_latency = (double)total_latency / NUM_ITERATIONS;

    double variance = 0.0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        variance += pow(latencies[i] - mean_latency, 2);
    }
    variance /= NUM_ITERATIONS;
    double stddev_latency = sqrt(variance);

    // Calculate 95% confidence interval
    double margin_of_error = 1.96 * stddev_latency / sqrt(NUM_ITERATIONS);

    // Output results
    printf("Total latency: %ld ns\n", total_latency);
    printf("Average latency: %.2f ns\n", mean_latency);
    printf("Minimum latency: %ld ns\n", min_latency);
    printf("Maximum latency: %ld ns\n", max_latency);
    printf("Standard deviation: %.2f ns\n", stddev_latency);
    printf("95%% Confidence interval: [%.2f ns, %.2f ns]\n", mean_latency - margin_of_error, mean_latency + margin_of_error);

    // Cleanup
    sem_destroy(&semaphore);

    return 0;
}
