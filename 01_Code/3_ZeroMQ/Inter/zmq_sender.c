#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../0_Bib/csv_writer.h"
#include "../0_Bib/statistics.h"

#define ITERATIONS 1000
#define MESSAGE_SIZE 10

uint64_t get_time_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main()
{
    void *context = zmq_ctx_new();
    if (context == NULL)
    {
        perror("Failed to create ZeroMQ context");
        exit(EXIT_FAILURE);
    }

    void *socket = zmq_socket(context, ZMQ_REQ);
    if (socket == NULL)
    {
        perror("Failed to create ZeroMQ socket");
        zmq_ctx_destroy(context);
        exit(EXIT_FAILURE);
    }

    if (zmq_connect(socket, "tcp://localhost:5555") != 0)
    {
        perror("Failed to connect to ZeroMQ socket");
        zmq_close(socket);
        zmq_ctx_destroy(context);
        exit(EXIT_FAILURE);
    }

    uint64_t start_time, end_time;
    uint64_t latencies[ITERATIONS];
    char buffer[MESSAGE_SIZE];

    // Vorwärmen von clock_gettime, um Initialisierungskosten zu vermeiden und präzisere Messungen sicherzustellen
    end_time = get_time_ns();
    start_time = get_time_ns();

    for (int i = 0; i < ITERATIONS; i++)
    {
        start_time = get_time_ns();
        zmq_send(socket, "ping", 4, 0);
        zmq_recv(socket, buffer, MESSAGE_SIZE, 0);
        end_time = get_time_ns();

        uint64_t latency = end_time - start_time;
        latencies[i] = latency;
    }

    zmq_close(socket);
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
    write_csv("03_zeromq_inter_latency.csv", latencies, ITERATIONS);

    return 0;
}
