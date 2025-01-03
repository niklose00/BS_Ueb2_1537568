#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <math.h>

#define NUM_ITERATIONS 1000

// Spinlocks
volatile atomic_flag spinlock_data = ATOMIC_FLAG_INIT; // Schutz des Zugriffs auf shared_data
volatile atomic_flag spinlock_sync = ATOMIC_FLAG_INIT; // Synchronisation zwischen Sender und Empfänger

// Gemeinsame Ressource
volatile int shared_data = 0;

// Funktion zur Zeitdifferenzberechnung
long calculate_elapsed_time(struct timespec start, struct timespec end) {
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    if (nanoseconds < 0) {
        seconds -= 1;
        nanoseconds += 1e9;
    }
    return seconds * 1e9 + nanoseconds;
}

// Lock- und Unlock-Funktionen
void lock(volatile atomic_flag *lock) {
    while (atomic_flag_test_and_set(lock)) {
        // Busy waiting
    }
}

void unlock(volatile atomic_flag *lock) {
    atomic_flag_clear(lock);
}

// Sender-Thread
void *sender(void *arg) {
    struct timespec *timing = (struct timespec *)arg;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        lock(&spinlock_sync);

        lock(&spinlock_data);
        clock_gettime(CLOCK_MONOTONIC_RAW, &timing[i * 2]); // Startzeit
        shared_data = i; // Simulierte Datenübertragung
        unlock(&spinlock_data);

        unlock(&spinlock_sync);
    }

    return NULL;
}

// Empfänger-Thread
void *receiver(void *arg) {
    struct timespec *timing = (struct timespec *)arg;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        while (atomic_flag_test_and_set(&spinlock_sync)) {
            // Busy waiting
        }

        lock(&spinlock_data);
        clock_gettime(CLOCK_MONOTONIC_RAW, &timing[i * 2 + 1]); // Endzeit
        int data = shared_data; // Simulierte Datenverarbeitung
        unlock(&spinlock_data);

        unlock(&spinlock_sync);
    }

    return NULL;
}

int main() {
    pthread_t sender_thread, receiver_thread;
    struct timespec timing[NUM_ITERATIONS * 2];
    long latencies[NUM_ITERATIONS];

    // Threads erstellen
    pthread_create(&sender_thread, NULL, sender, timing);
    pthread_create(&receiver_thread, NULL, receiver, timing);

    // Auf Threads warten
    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    // Latenzen berechnen
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        latencies[i] = calculate_elapsed_time(timing[i * 2], timing[i * 2 + 1]);
    }

    // Statistiken berechnen
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

    double margin_of_error = 1.96 * stddev_latency / sqrt(NUM_ITERATIONS);

    // Ergebnisse ausgeben
    printf("Total latency: %ld ns\n", total_latency);
    printf("Average latency: %.2f ns\n", mean_latency);
    printf("Minimum latency: %ld ns\n", min_latency);
    printf("Maximum latency: %ld ns\n", max_latency);
    printf("Standard deviation: %.2f ns\n", stddev_latency);
    printf("95%% Confidence interval: [%.2f ns, %.2f ns]\n", mean_latency - margin_of_error, mean_latency + margin_of_error);

    return 0;
}
