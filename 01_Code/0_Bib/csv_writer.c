#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "csv_writer.h"

// Hilfsfunktion, um sicherzustellen, dass der Ordner existiert
void ensure_directory_exists(const char *dir)
{
    printf("Überprüfe Verzeichnis: %s\n", dir); // Ausgabe des Pfades
    
    struct stat st = {0};
    if (stat(dir, &st) == -1)
    {
        printf("Verzeichnis existiert nicht. Erstelle: %s\n", dir);
        
        if (mkdir(dir, 0777) != 0)
        {
            perror("Fehler beim Erstellen des Ordners");
            exit(EXIT_FAILURE);
        }
        
        printf("Verzeichnis erstellt: %s\n", dir);
    }
    else
    {
        printf("Verzeichnis existiert bereits: %s\n", dir);
    }
}


void write_csv(const char *filename, uint64_t *data, size_t size, const char *option)
{
    const char *output_dir = "../../../02_Outputs/Csv";

    if (option != NULL && (strcmp(option, "2") == 0 || strcmp(option, "docker") == 0))
    {
        output_dir = "./02_Outputs/Csv";
    }


    ensure_directory_exists(output_dir);

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", output_dir, filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL)
    {
        perror("Fehler beim Öffnen der Datei");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i++)
    {
        fprintf(file, "%lu\n", data[i]);
    }

    fclose(file);
}
