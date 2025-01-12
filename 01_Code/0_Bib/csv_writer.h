#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>


// Funktionsdeklaration
void write_csv(const char *filename, uint64_t *data, size_t size, const char *option);


#endif
