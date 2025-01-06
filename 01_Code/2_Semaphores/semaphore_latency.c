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
double min_latency = DBL_MAX; // Initial minimal latency

#define ITERATIONS 1000 // Anzahl der Messungen

// Funktion, die der zweite Thread ausführt
void* thread_func(void* arg) {
    // Warten auf die Semaphore
    sem_wait(&semaphore);

    // Endzeit messen
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    return NULL;
}

int main() {
    pthread_t thread;
    uint64_t latencies[ITERATIONS]; // Array für Latenzen

    // Semaphore initialisieren
    if (sem_init(&semaphore, 0, 0) != 0) {
        perror("Semaphore initialization failed");
        return EXIT_FAILURE;
    }

    // Thread erstellen
    if (pthread_create(&thread, NULL, thread_func, NULL) != 0) {
        perror("Thread creation failed");
        return EXIT_FAILURE;
    }

    // Mehrfache Messungen durchführen
    for (int i = 0; i < ITERATIONS; i++) {
        // Startzeit messen
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        // Semaphore setzen
        sem_post(&semaphore);

        // Warten auf den Abschluss des Threads
        pthread_join(thread, NULL);

        // Latenz berechnen
        double latency = (end_time.tv_sec - start_time.tv_sec) * 1e9 +
                         (end_time.tv_nsec - start_time.tv_nsec);

        latencies[i] = latency;

        // Minimalwert aktualisieren
        if (latency < min_latency) {
            min_latency = latency;
        }

        // Neuen Thread für die nächste Iteration erstellen
        if (pthread_create(&thread, NULL, thread_func, NULL) != 0) {
            perror("Thread recreation failed");
            return EXIT_FAILURE;
        }
    }

    // Semaphore zerstören
    sem_destroy(&semaphore);

    // Statistik berechnen
    double sum = 0.0, mean, stddev = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += latencies[i];
    }
    mean = sum / ITERATIONS;

    for (int i = 0; i < ITERATIONS; i++) {
        stddev += (latencies[i] - mean) * (latencies[i] - mean);
    }
    stddev = sqrt(stddev / ITERATIONS);

    // 95%-Konfidenzintervall berechnen
    double z = 1.96; // z-Wert für 95%-Konfidenzintervall
    double confidence_interval = z * (stddev / sqrt(ITERATIONS));

    // Ergebnisse ausgeben
    printf("Ergebnisse:\n");
    printf("Minimal gemessene Latenz: %.2f ns\n", min_latency);
    printf("Mittlere Latenz: %.2f ns\n", mean);
    printf("Standardabweichung: %.2f ns\n", stddev);
    printf("95%%-Konfidenzintervall: [%.2f, %.2f] ns\n",
           mean - confidence_interval, mean + confidence_interval);

    // CSV-Datei erstellen
    write_csv("02_semaphores_latencies.csv", latencies, ITERATIONS);

    return EXIT_SUCCESS;
}
