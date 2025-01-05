#include <stdio.h>
#include <stdlib.h>
#include "csv_writer.h"

void write_csv(const char *filename, uint64_t *data, size_t size) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Fehler beim Öffnen der Datei");
        exit(EXIT_FAILURE);
    }

    // Kopfzeile schreiben
    fprintf(file, "Latenz (ns)\n");

    // Daten schreiben
    for (size_t i = 0; i < size; i++) {
        fprintf(file, "%lu\n", data[i]);
    }

    fclose(file);
}
