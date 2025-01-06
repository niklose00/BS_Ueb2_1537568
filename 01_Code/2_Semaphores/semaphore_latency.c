#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include "../0_Bib/csv_writer.h"
#include "../0_Bib/statistics.h"

// Globale Variablen
sem_t semaphore;
struct timespec start_time, end_time;

#define ITERATIONS 1000

uint64_t get_time_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

// Funktion, die der zweite Thread ausführt
void *thread_func(void *arg)
{
    // Warten auf die Semaphore
    sem_wait(&semaphore);

    // Endzeit messen
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    return NULL;
}

int main()
{
    pthread_t thread;
    uint64_t latencies[ITERATIONS];

    // Semaphore initialisieren
    if (sem_init(&semaphore, 0, 0) != 0)
    {
        perror("Semaphore initialization failed");
        return EXIT_FAILURE;
    }

    // Thread erstellen
    if (pthread_create(&thread, NULL, thread_func, NULL) != 0)
    {
        perror("Thread creation failed");
        return EXIT_FAILURE;
    }

    uint64_t start_time, end_time;

    // Vorwärmen von clock_gettime, um Initialisierungskosten zu vermeiden und präzisere Messungen sicherzustellen
    end_time = get_time_ns();
    start_time = get_time_ns();

    // Mehrfache Messungen durchführen
    for (int i = 0; i < ITERATIONS; i++)
    {
        start_time = get_time_ns();

        // Semaphore setzen
        sem_post(&semaphore);

        // Warten auf den Abschluss des Threads
        pthread_join(thread, NULL);

        end_time = get_time_ns();

        uint64_t latency = end_time - start_time;
        latencies[i] = latency;

        // Neuen Thread für die nächste Iteration erstellen
        if (pthread_create(&thread, NULL, thread_func, NULL) != 0)
        {
            perror("Thread recreation failed");
            return EXIT_FAILURE;
        }
    }

    // Semaphore zerstören
    sem_destroy(&semaphore);

    // Konvertiere latencies in double-Werte
    double latencies_double[ITERATIONS];
    for (int i = 0; i < ITERATIONS; i++)
    {
        latencies_double[i] = (double)latencies[i];
    }

    // Statistik berechnen
    double min, max, mean, stddev, ci_lower, ci_upper;
    calculate_statistics(latencies_double, ITERATIONS, &min, &max, &mean, &stddev, &ci_lower, &ci_upper);

    // Ergebnisse ausgeben
    printf("Minimale Latenz: %.2f ns\n", min);
    printf("Maximale Latenz: %.2f ns\n", max);
    printf("Mittlere Latenz: %.2f ns\n", mean);
    printf("Standardabweichung: %.2f ns\n", stddev);
    printf("95%%-Konfidenzintervall: [%.2f ns, %.2f ns]\n", ci_lower, ci_upper);

    // CSV-Datei erstellen
    write_csv("02_semaphores_latencies.csv", latencies, ITERATIONS);

    return EXIT_SUCCESS;
}
