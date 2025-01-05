#include <stdio.h>
#include <string.h>
#include <zmq.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define NUM_ITERATIONS 1000

long calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

double calculate_mean(long *data, int size) {
    long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return (double)sum / size;
}

double calculate_standard_deviation(long *data, int size, double mean) {
    double sum_squared_diff = 0;
    for (int i = 0; i < size; i++) {
        sum_squared_diff += pow(data[i] - mean, 2);
    }
    return sqrt(sum_squared_diff / size);
}

int compare(const void *a, const void *b) {
    return (*(long *)a - *(long *)b);
}

int main() {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(socket, "tcp://receiver:5555");

    struct timespec start, end;
    long latencies[NUM_ITERATIONS];
    long total_time = 0;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        zmq_send(socket, "ping", 4, 0);

        char buffer[10];
        zmq_recv(socket, buffer, 10, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);

        latencies[i] = calculate_elapsed_time(start, end);
        total_time += latencies[i];
    }

    // Sort the latencies to find min and max
    qsort(latencies, NUM_ITERATIONS, sizeof(long), compare);

    long min_latency = latencies[0];
    long max_latency = latencies[NUM_ITERATIONS - 1];
    double mean_latency = calculate_mean(latencies, NUM_ITERATIONS);
    double stddev = calculate_standard_deviation(latencies, NUM_ITERATIONS, mean_latency);

    // Calculate 95% confidence interval
    double margin_of_error = 1.96 * (stddev / sqrt(NUM_ITERATIONS));
    double lower_bound = mean_latency - margin_of_error;
    double upper_bound = mean_latency + margin_of_error;

    printf("Total time: %ld ns\n", total_time);
    printf("Average latency: %.2f ns\n", mean_latency);
    printf("Min latency: %ld ns\n", min_latency);
    printf("Max latency: %ld ns\n", max_latency);
    printf("95%% Confidence Interval: [%.2f, %.2f] ns\n", lower_bound, upper_bound);

    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}
