CC = gcc
CFLAGS = -pthread -lm -I./0_Bib
BUILD_DIR = bin

# Ziele
TARGET = spinlock_latency
TARGET2 = semaphore_latency
TARGET3_1 = zeromq_intra_latency
TARGET3_2_sender = zeromq_inter_sender_latency
TARGET3_2_receiver = zeromq_inter_receiver_latency

# Gemeinsame Dateien
COMMON_SRC = 0_Bib/csv_writer.c 0_Bib/statistics.c
COMMON_OBJ = $(COMMON_SRC:.c=.o)

# Quellen für jedes Ziel
SRC = 1_Spinlocks/spinlock_latency.c
SRC2 = 2_Semaphores/semaphore_latency.c
SRC3_1 = 3_ZeroMQ/Intra/zeromq_latency.c
SRC3_2_sender = 3_ZeroMQ/Inter/zmq_sender.c
SRC3_2_receiver = 3_ZeroMQ/Inter/zmq_receiver.c

OBJ = $(SRC:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJ3_1 = $(SRC3_1:.c=.o)
OBJ3_2_sender = $(SRC3_2_sender:.c=.o)
OBJ3_2_receiver = $(SRC3_2_receiver:.c=.o)

# Standardziel
all: $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/$(TARGET2) $(BUILD_DIR)/$(TARGET3_1) $(BUILD_DIR)/$(TARGET3_2_sender) $(BUILD_DIR)/$(TARGET3_2_receiver)

# Ziel 1
$(BUILD_DIR)/$(TARGET): $(OBJ) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# Ziel 2
$(BUILD_DIR)/$(TARGET2): $(OBJ2) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# Ziel 3_1
$(BUILD_DIR)/$(TARGET3_1): $(OBJ3_1) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS) -lzmq

# Ziel 3_2
#sender
$(BUILD_DIR)/$(TARGET3_2_sender): $(OBJ3_2_sender) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS) -lzmq

#receiver
$(BUILD_DIR)/$(TARGET3_2_receiver): $(OBJ3_2_receiver) $(COMMON_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(CFLAGS) -lzmq

# Regel für .o-Dateien
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Bereinigen
clean:
	rm -rf $(BUILD_DIR) *.o
