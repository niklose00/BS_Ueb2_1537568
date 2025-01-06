CC = gcc
CFLAGS = -pthread -lm -I./0_Bib
BUILD_DIR = bin

# Ziele
TARGET = spinlock_latency
TARGET2 = semaphore_latency
TARGET3_1 = zeromq_intra_latency

# Gemeinsame Dateien
COMMON_SRC = 0_Bib/csv_writer.c 0_Bib/statistics.c
COMMON_OBJ = $(COMMON_SRC:.c=.o)

# Quellen für jedes Ziel
SRC = 1_Spinlocks/spinlock_latency.c
SRC2 = 2_Semaphores/semaphore_latency.c
SRC3_1 = 3_ZeroMQ/Intra/zeromq_latency.c

OBJ = $(SRC:.c=.o)
OBJ2 = $(SRC2:.c=.o)

# Standardziel
all: $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/$(TARGET2) $(BUILD_DIR)/$(TARGET3_1)

# Ziel 1
$(BUILD_DIR)/$(TARGET): $(OBJ) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# Ziel 2
$(BUILD_DIR)/$(TARGET2): $(OBJ2) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# Ziel 3_1
$(BUILD_DIR)/$(TARGET3_1): $(OBJ2) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# Regel für .o-Dateien
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Bereinigen
clean:
	rm -rf $(BUILD_DIR) *.o
