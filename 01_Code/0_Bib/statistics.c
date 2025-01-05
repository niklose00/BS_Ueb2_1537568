#include <stdio.h>
#include <math.h>
#include "statistics.h"


void calculate_statistics(
    const double *data,
    size_t size,
    double *min,
    double *max,
    double *mean,
    double *stddev,
    double *ci_lower,
    double *ci_upper
) {
    if (data == NULL || size == 0) {
        fprintf(stderr, "Fehler: Ungültige Eingabedaten\n");
        return;
    }

    *min = data[0];
    *max = data[0];
    double sum = 0.0;

    // Erster Durchgang: min, max, mean
    for (size_t i = 0; i < size; i++) {
        if (data[i] < *min) *min = data[i];
        if (data[i] > *max) *max = data[i];
        sum += data[i];
    }
    *mean = sum / size;

    // Zweiter Durchgang: Standardabweichung
    double variance = 0.0;
    for (size_t i = 0; i < size; i++) {
        variance += (data[i] - *mean) * (data[i] - *mean);
    }
    variance /= size;
    *stddev = sqrt(variance);

    // 95%-Konfidenzintervall
    double ci = 1.96 * (*stddev) / sqrt(size);
    *ci_lower = *mean - ci;
    *ci_upper = *mean + ci;
}
