#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "../0_Bib/csv_writer.h"


#define ITERATIONS 10

volatile int spinlock = 0;

// Funktion zum Ermitteln der aktuellen Zeit in Nanosekunden
uint64_t get_time_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

// Spinlock-Implementierung
void acquire_spinlock(volatile int *lock)
{
    while (__sync_lock_test_and_set(lock, 1))
    {
        // Busy Waiting
    }
}

void release_spinlock(volatile int *lock)
{
    __sync_lock_release(lock);
}

// Thread-Funktion
void *thread_function(void *arg)
{
    int *shared_flag = (int *)arg;
    for (int i = 0; i < ITERATIONS; i++)
    {
        acquire_spinlock(&spinlock);
        (*shared_flag)++;
        release_spinlock(&spinlock);
    }
    return NULL;
}


int main()
{
    pthread_t thread1, thread2;
    int shared_flag = 0;

    // Erstellen der Threads
    if (pthread_create(&thread1, NULL, thread_function, &shared_flag) != 0)
    {
        perror("Fehler beim Erstellen von Thread 1");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread2, NULL, thread_function, &shared_flag) != 0)
    {
        perror("Fehler beim Erstellen von Thread 2");
        exit(EXIT_FAILURE);
    }

    uint64_t start_time, end_time;
    uint64_t min_latency = UINT64_MAX;
    uint64_t latencies[ITERATIONS];

    // Messen der Latenzzeiten
    for (int i = 0; i < ITERATIONS; i++)
    {
        start_time = get_time_ns();
        acquire_spinlock(&spinlock);
        release_spinlock(&spinlock);
        end_time = get_time_ns();

        printf("START: %lu ns\n", start_time); // Korrekte Ausgabe
        printf("ENDE: %lu ns\n", end_time);    // Korrekte Ausgabe

        uint64_t latency = end_time - start_time;
        latencies[i] = latency;
        if (latency < min_latency)
        {
            min_latency = latency;
        }
    }

    // Warten auf die Threads
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Berechnung des Mittelwerts und der Standardabweichung
    double sum = 0.0, mean, stddev = 0.0;
    for (int i = 0; i < ITERATIONS; i++)
    {
        sum += latencies[i];
        printf("test%lu\n", latencies[i]);
    }
    mean = sum / ITERATIONS;

    for (int i = 0; i < ITERATIONS; i++)
    {
        stddev += (latencies[i] - mean) * (latencies[i] - mean);
    }
    stddev = sqrt(stddev / ITERATIONS);

    // Konfidenzintervall (95%)
    double conf_interval = 1.96 * stddev / sqrt(ITERATIONS);

    // Ergebnisse ausgeben
    printf("Minimale Latenz: %lu ns\n", min_latency);
    printf("Mittlere Latenz: %.2f ns\n", mean);
    printf("Standardabweichung: %.2f ns\n", stddev);
    printf("95%%-Konfidenzintervall: [%.2f ns, %.2f ns]\n", mean - conf_interval, mean + conf_interval);

    // CSV-Datei erstellen
    write_csv("01_spinlock_latencies.csv", latencies, ITERATIONS);
    
    return 0;
}
