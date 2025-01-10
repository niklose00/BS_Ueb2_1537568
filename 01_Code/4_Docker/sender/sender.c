#include <stdio.h>
#include <string.h>
#include <zmq.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "./csv_writer.h"
#include "./statistics.h"

#define ITERATIONS 1000

uint64_t get_time_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main()
{
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(socket, "tcp://receiver:5555");

    uint64_t start_time, end_time;
    long latencies[ITERATIONS];
    long total_time = 0;

    // Vorwärmen von clock_gettime, um Initialisierungskosten zu vermeiden und präzisere Messungen sicherzustellen
    end_time = get_time_ns();
    start_time = get_time_ns();

    for (int i = 0; i < ITERATIONS; i++)
    {
        start_time = get_time_ns();
        zmq_send(socket, "ping", 4, 0);

        char buffer[10];
        zmq_recv(socket, buffer, 10, 0);
        end_time = get_time_ns();

        uint64_t latency = end_time - start_time;
        latencies[i] = latency;
    }

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
    write_csv("04_docker_latencies.csv", latencies, ITERATIONS);

    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}
