CC = gcc
CFLAGS = -pthread -lm -I./0_Bib
TARGET = spinlock_latency
SRC = 1_Spinlocks/spinlock_latency.c 0_Bib/csv_writer.c 0_Bib/statistics.c
BUILD_DIR = bin

# Standardziel: Bauen und in den Zielordner verschieben
all: $(BUILD_DIR)/$(TARGET)

# Ziel für die ausführbare Datei im Zielordner
$(BUILD_DIR)/$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)         # Erstelle das Zielverzeichnis, falls es nicht existiert
	$(CC) -o $@ $(SRC) $(CFLAGS)  # Kompiliere und speichere im Zielordner

# Bereinigen
clean:
	rm -rf $(BUILD_DIR)
