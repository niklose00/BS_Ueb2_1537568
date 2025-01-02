#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <pthread.h>
#include <time.h>

#define NUM_ITERATIONS 1000

// Timing function
long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

// Sender thread for intra-process communication
void *intra_process_sender(void *context) {
    void *sender = zmq_socket(context, ZMQ_REQ);
    if (zmq_connect(sender, "inproc://latency_test") != 0) {
        perror("zmq_connect failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        zmq_send(sender, "ping", 4, 0);
        char buffer[10];
        zmq_recv(sender, buffer, 10, 0);
    }

    zmq_close(sender);
    return NULL;
}

// Receiver thread for intra-process communication
void *intra_process_receiver(void *context) {
    void *receiver = zmq_socket(context, ZMQ_REP);
    if (zmq_bind(receiver, "inproc://latency_test") != 0) {
        perror("zmq_bind failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        char buffer[10];
        zmq_recv(receiver, buffer, 10, 0);
        zmq_send(receiver, "pong", 4, 0);
    }

    zmq_close(receiver);
    return NULL;
}

int main() {
    struct timespec start, end;

    // Intra-process communication setup
    printf("Intra-process communication:\n");
    void *context = zmq_ctx_new();

    pthread_t sender_thread, receiver_thread;

    // Start timing
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads
    pthread_create(&receiver_thread, NULL, intra_process_receiver, context);
    pthread_create(&sender_thread, NULL, intra_process_sender, context);

    // Wait for threads to finish
    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    // End timing
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time
    long elapsed_time = calculate_elapsed_time(start, end);
    double average_latency = (double)elapsed_time / (NUM_ITERATIONS * 2);

    // Output results
    printf("Total time: %ld ns\n", elapsed_time);
    printf("Average latency per message: %.2f ns\n", average_latency);

    zmq_ctx_destroy(context);

    return 0;
}
