#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_ITERATIONS 1000

long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

int main() {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(socket, "tcp://localhost:5555");

    struct timespec start, end;
    long total_time = 0;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        zmq_send(socket, "ping", 4, 0);

        char buffer[10];
        zmq_recv(socket, buffer, 10, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);

        total_time += calculate_elapsed_time(start, end);
    }

    printf("Total time: %ld ns\n", total_time);
    printf("Average latency per message: %.2f ns\n", (double)total_time / NUM_ITERATIONS);

    zmq_close(socket);
    zmq_ctx_destroy(context);
    return 0;
}



