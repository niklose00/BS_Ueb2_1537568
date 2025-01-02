#include <zmq.h>
#include <stdio.h>

int main() {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REP);
    zmq_bind(socket, "tcp://*:5555");

    for (int i = 0; i < 1000; i++) {
        char buffer[10];
        zmq_recv(socket, buffer, 10, 0);
        zmq_send(socket, "pong", 4, 0);
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);
    return 0;
}