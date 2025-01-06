#include <stdio.h>
#include <pthread.h>
#include <zmq.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "../0_Bib/csv_writer.h"
#include "../0_Bib/statistics.h"

#define ITERATIONS 10000

uint64_t latencies[ITERATIONS];

// Funktion zum Ermitteln der aktuellen Zeit in Nanosekunden
uint64_t get_time_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

void *sender(void *context)
{
    void *socket = zmq_socket(context, ZMQ_PAIR);
    zmq_bind(socket, "inproc://latency_test");

    struct timespec start_time;
    for (int i = 0; i < ITERATIONS; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &start_time); // Startzeit erfassen
        zmq_send(socket, &start_time, sizeof(start_time), 0);
    }

    zmq_close(socket);
    return NULL;
}

void *receiver(void *context)
{
    void *socket = zmq_socket(context, ZMQ_PAIR);
    zmq_connect(socket, "inproc://latency_test");

    struct timespec start_time, end_time;
    for (int i = 0; i < ITERATIONS; i++)
    {
        zmq_recv(socket, &start_time, sizeof(start_time), 0); // Startzeit empfangen
        clock_gettime(CLOCK_MONOTONIC, &end_time);            // Endzeit erfassen

        // Latenz berechnen und speichern
        double latency = (end_time.tv_sec - start_time.tv_sec) * 1e6 +
                         (end_time.tv_nsec - start_time.tv_nsec) / 1e3; // Latenz in Mikrosekunden
        latencies[i] = latency;
    }

    zmq_close(socket);
    return NULL;
}

int main()
{
    void *context = zmq_ctx_new();

    pthread_t sender_thread, receiver_thread;

    pthread_create(&sender_thread, NULL, sender, context);
    pthread_create(&receiver_thread, NULL, receiver, context);

    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    zmq_ctx_destroy(context);

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
    write_csv("03_zeromq_intra_latency.csv", latencies, ITERATIONS);

    return 0;
}
