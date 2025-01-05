#ifndef STATISTICS_H
#define STATISTICS_H

#include <stddef.h>

void calculate_statistics(
    const double *data,
    size_t size,
    double *min,
    double *max,
    double *mean,
    double *stddev,
    double *ci_lower,
    double *ci_upper
);

#endif
