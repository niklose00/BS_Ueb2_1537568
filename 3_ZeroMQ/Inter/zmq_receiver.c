#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_ITERATIONS 1000
#define MESSAGE_SIZE 10

int main() {
    void *context = zmq_ctx_new();
    if (context == NULL) {
        perror("Failed to create ZeroMQ context");
        exit(EXIT_FAILURE);
    }

    void *socket = zmq_socket(context, ZMQ_REP);
    if (socket == NULL) {
        perror("Failed to create ZeroMQ socket");
        zmq_ctx_destroy(context);
        exit(EXIT_FAILURE);
    }

    if (zmq_bind(socket, "tcp://*:5555") != 0) {
        perror("Failed to bind ZeroMQ socket");
        zmq_close(socket);
        zmq_ctx_destroy(context);
        exit(EXIT_FAILURE);
    }

    char buffer[MESSAGE_SIZE];
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        zmq_recv(socket, buffer, MESSAGE_SIZE, 0);
        zmq_send(socket, "pong", 4, 0);
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);
    return 0;
}
