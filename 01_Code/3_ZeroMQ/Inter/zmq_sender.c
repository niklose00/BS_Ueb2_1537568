#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define NUM_ITERATIONS 1000
#define MESSAGE_SIZE 10

long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

void calculate_statistics(long *latencies, int size, double *mean, double *stddev, double *ci_lower, double *ci_upper) {
    long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += latencies[i];
    }
    *mean = (double)sum / size;

    double variance = 0.0;
    for (int i = 0; i < size; i++) {
        variance += pow(latencies[i] - *mean, 2);
    }
    variance /= size;
    *stddev = sqrt(variance);

    double z = 1.96; // z-Wert für 95%-Konfidenzintervall
    double margin_of_error = z * (*stddev / sqrt(size));
    *ci_lower = *mean - margin_of_error;
    *ci_upper = *mean + margin_of_error;
}

int main() {
    void *context = zmq_ctx_new();
    if (context == NULL) {
        perror("Failed to create ZeroMQ context");
        exit(EXIT_FAILURE);
    }

    void *socket = zmq_socket(context, ZMQ_REQ);
    if (socket == NULL) {
        perror("Failed to create ZeroMQ socket");
        zmq_ctx_destroy(context);
        exit(EXIT_FAILURE);
    }

    if (zmq_connect(socket, "tcp://localhost:5555") != 0) {
        perror("Failed to connect to ZeroMQ socket");
        zmq_close(socket);
        zmq_ctx_destroy(context);
        exit(EXIT_FAILURE);
    }

    struct timespec start, end;
    long latencies[NUM_ITERATIONS];
    char buffer[MESSAGE_SIZE];

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        zmq_send(socket, "ping", 4, 0);
        zmq_recv(socket, buffer, MESSAGE_SIZE, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);

        latencies[i] = calculate_elapsed_time(start, end);
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);

    // Statistische Analyse
    double mean, stddev, ci_lower, ci_upper;
    calculate_statistics(latencies, NUM_ITERATIONS, &mean, &stddev, &ci_lower, &ci_upper);

    printf("Minimale Latenz: %ld ns\n", latencies[0]);
    printf("Mittlere Latenz: %.2f ns\n", mean);
    printf("Standardabweichung: %.2f ns\n", stddev);
    printf("95%% Konfidenzintervall: [%.2f, %.2f] ns\n", ci_lower, ci_upper);

    return 0;
}
