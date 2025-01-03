#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <math.h>

#define NUM_ITERATIONS 1000

// Define the Spinlock
volatile atomic_flag spinlock_data = ATOMIC_FLAG_INIT; // Für geteilte Daten
volatile atomic_flag spinlock_sync = ATOMIC_FLAG_INIT; // Für Synchronisation


// Shared data
volatile int shared_data = 0;

//Gegen race condition
volatile int ready_flag = 0; // 0 = Sender noch nicht fertig, 1 = Sender fertig


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
    struct timespec *timing = (struct timespec *)arg;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // Sperre Synchronisation
        lock(&spinlock_sync);

        // Sperre Datenzugriff
        lock(&spinlock_data);
        clock_gettime(CLOCK_MONOTONIC_RAW, &timing[i * 2]); // Startzeit
        shared_data = i; // Simuliere Datenübertragung
        unlock(&spinlock_data);

        // Entsperre Synchronisation für den Empfänger
        unlock(&spinlock_sync);
    }

    return NULL;
}



void *receiver(void *arg) {
    struct timespec *timing = (struct timespec *)arg;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // Warte auf Synchronisations-Signal
        while (atomic_flag_test_and_set(&spinlock_sync)) {
            // Busy waiting
        }

        // Sperre Datenzugriff
        lock(&spinlock_data);
        clock_gettime(CLOCK_MONOTONIC_RAW, &timing[i * 2 + 1]); // Endzeit
        int data = shared_data; // Simuliere Datenverarbeitung
        unlock(&spinlock_data);

        // Entsperre Synchronisation für den Sender
        unlock(&spinlock_sync);
    }

    return NULL;
}


int main() {
    pthread_t sender_thread, receiver_thread;
    struct timespec timing[NUM_ITERATIONS * 2];
    long latencies[NUM_ITERATIONS];

    // Create threads
    pthread_create(&sender_thread, NULL, sender, timing);
    pthread_create(&receiver_thread, NULL, receiver, timing);

    // Wait for threads to finish
    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    // Debug: Print timing values with differences
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        long diff = calculate_elapsed_time(timing[i * 2], timing[i * 2 + 1]);
        printf("Iteration %d: Start: %ld.%09ld, End: %ld.%09ld, Diff: %ld ns\n",
               i, timing[i * 2].tv_sec, timing[i * 2].tv_nsec,
               timing[i * 2 + 1].tv_sec, timing[i * 2 + 1].tv_nsec, diff);
    }

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
        // printf("total: %ld\n", total_latency);

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

    return 0;
}
