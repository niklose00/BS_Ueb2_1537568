#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <zmq.h>
#include <math.h>
#include <sys/wait.h>

#define NUM_ITERATIONS 10000

double latencies[NUM_ITERATIONS]; // Array zur Speicherung der Latenzen

void sender_process() {
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_PAIR);

    int linger = 0;
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));

    if (zmq_bind(socket, "ipc:///tmp/zeromq_latency") != 0) {
        perror("Sender: zmq_bind failed");
        exit(1);
    }

    // Warten auf READY-Nachricht vom Empfänger
    char buffer[10];
    if (zmq_recv(socket, buffer, sizeof(buffer), 0) == -1) {
        perror("Sender: Failed to receive READY signal");
        exit(1);
    }
    printf("Sender: Empfänger ist bereit\n");

    struct timespec start_time;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
        if (zmq_send(socket, &start_time, sizeof(start_time), 0) == -1) {
            perror("Sender: zmq_send failed");
            exit(1);
        }
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);
}

void receiver_process() {
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_PAIR);

    int linger = 0;
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));

    if (zmq_connect(socket, "ipc:///tmp/zeromq_latency") != 0) {
        perror("Receiver: zmq_connect failed");
        exit(1);
    }

    // READY-Nachricht an den Sender senden
    const char* ready_msg = "READY";
    if (zmq_send(socket, ready_msg, strlen(ready_msg), 0) == -1) {
        perror("Receiver: Failed to send READY signal");
        exit(1);
    }
    printf("Receiver: READY signal gesendet\n");

    struct timespec start_time, end_time;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        if (zmq_recv(socket, &start_time, sizeof(start_time), 0) == -1) {
            perror("Receiver: zmq_recv failed");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);

        // Latenz in Mikrosekunden berechnen
        double latency = (end_time.tv_sec - start_time.tv_sec) * 1e6 +
                         (end_time.tv_nsec - start_time.tv_nsec) / 1e3;
        latencies[i] = latency;
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);

    // Statistische Analyse
    double sum = 0.0, min_latency = latencies[0];
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sum += latencies[i];
        if (latencies[i] < min_latency) {
            min_latency = latencies[i];
        }
    }
    double mean = sum / NUM_ITERATIONS;

    // Standardabweichung berechnen
    double variance_sum = 0.0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        variance_sum += pow(latencies[i] - mean, 2);
    }
    double stddev = sqrt(variance_sum / NUM_ITERATIONS);

    // Konfidenzintervall (95%) berechnen
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
    pid_t pid = fork();

    if (pid == 0) {
        // Kindprozess (Empfänger)
        receiver_process();
    } else if (pid > 0) {
        // Elternprozess (Sender)
        sender_process();
        wait(NULL); // Warten auf Kindprozess
    } else {
        perror("fork failed");
        exit(1);
    }

    return 0;
}
