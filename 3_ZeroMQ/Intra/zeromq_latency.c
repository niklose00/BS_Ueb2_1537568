#include <stdio.h>
#include <pthread.h>
#include <zmq.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define NUM_ITERATIONS 10000

double latencies[NUM_ITERATIONS]; // Array zur Speicherung der Latenzen

void* sender(void* context) {
    void* socket = zmq_socket(context, ZMQ_PAIR);
    zmq_bind(socket, "inproc://latency_test");

    struct timespec start_time;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start_time); // Startzeit erfassen
        zmq_send(socket, &start_time, sizeof(start_time), 0);
    }

    zmq_close(socket);
    return NULL;
}

void* receiver(void* context) {
    void* socket = zmq_socket(context, ZMQ_PAIR);
    zmq_connect(socket, "inproc://latency_test");

    struct timespec start_time, end_time;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        zmq_recv(socket, &start_time, sizeof(start_time), 0); // Startzeit empfangen
        clock_gettime(CLOCK_MONOTONIC, &end_time);           // Endzeit erfassen

        // Latenz berechnen und speichern
        double latency = (end_time.tv_sec - start_time.tv_sec) * 1e6 +
                         (end_time.tv_nsec - start_time.tv_nsec) / 1e3; // Latenz in Mikrosekunden
        latencies[i] = latency;
    }

    zmq_close(socket);
    return NULL;
}

void calculate_statistics() {
    double sum = 0.0, mean, stddev, min_latency = latencies[0];
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sum += latencies[i];
        if (latencies[i] < min_latency) {
            min_latency = latencies[i];
        }
    }
    mean = sum / NUM_ITERATIONS;

    // Standardabweichung berechnen
    double variance_sum = 0.0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        variance_sum += pow(latencies[i] - mean, 2);
    }
    stddev = sqrt(variance_sum / NUM_ITERATIONS);

    // Konfidenzintervall berechnen
    double z = 1.96; // z-Wert für 95% Konfidenzintervall
    double margin_of_error = z * (stddev / sqrt(NUM_ITERATIONS));
    double ci_lower = mean - margin_of_error;
    double ci_upper = mean + margin_of_error;

    // Ergebnisse ausgeben
    printf("Minimale Latenz: %.2f µs\n", min_latency);
    printf("Mittlere Latenz: %.2f µs\n", mean);
    printf("Standardabweichung: %.2f µs\n", stddev);
    printf("95%% Konfidenzintervall: [%.2f, %.2f] µs\n", ci_lower, ci_upper);
}

int main() {
    void* context = zmq_ctx_new();

    pthread_t sender_thread, receiver_thread;

    pthread_create(&sender_thread, NULL, sender, context);
    pthread_create(&receiver_thread, NULL, receiver, context);

    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    zmq_ctx_destroy(context);

    calculate_statistics();

    return 0;
}
