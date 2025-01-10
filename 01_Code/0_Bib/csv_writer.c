#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "csv_writer.h"

// Hilfsfunktion, um sicherzustellen, dass der Ordner existiert
void ensure_directory_exists(const char *dir) {
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        // Ordner erstellen
        if (mkdir(dir, 0777) != 0) {
            perror("Fehler beim Erstellen des Ordners");
            exit(EXIT_FAILURE);
        }
    }
}

void write_csv(const char *filename, uint64_t *data, size_t size) {
    const char *output_dir = "../../../02_Outputs/Csv";  // Ordner im Root-Verzeichnis
    ensure_directory_exists(output_dir);      // Sicherstellen, dass der Ordner existiert

    // Dynamischer Pfad zur Datei im Ordner "02_Outputs"
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", output_dir, filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Fehler beim Öffnen der Datei");
        exit(EXIT_FAILURE);
    }

    // Daten schreiben
    for (size_t i = 0; i < size; i++) {
        fprintf(file, "%lu\n", data[i]);
    }

    fclose(file);
}
