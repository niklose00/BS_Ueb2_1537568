CC = gcc
CFLAGS = -pthread -lm -I./0_Bib
BUILD_DIR = bin
OBJ_DIR = $(BUILD_DIR)/objects
EXEC_DIR = $(BUILD_DIR)/executables

# Ziele
TARGET = spinlock_latency
TARGET2 = semaphore_latency
TARGET3_1 = zeromq_intra_latency
TARGET3_2_sender = zeromq_inter_sender_latency
TARGET3_2_receiver = zeromq_inter_receiver_latency

# Gemeinsame Dateien
COMMON_SRC = 0_Bib/csv_writer.c 0_Bib/statistics.c
COMMON_OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(COMMON_SRC))

# Quellen für jedes Ziel
SRC = 1_Spinlocks/spinlock_latency.c
SRC2 = 2_Semaphores/semaphore_latency.c
SRC3_1 = 3_ZeroMQ/Intra/zeromq_latency.c
SRC3_2_sender = 3_ZeroMQ/Inter/zmq_sender.c
SRC3_2_receiver = 3_ZeroMQ/Inter/zmq_receiver.c

# Objektdateien (mit Pfad zu $(OBJ_DIR))
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
OBJ2 = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC2))
OBJ3_1 = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC3_1))
OBJ3_2_sender = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC3_2_sender))
OBJ3_2_receiver = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC3_2_receiver))

# Standardziel
all: $(EXEC_DIR)/$(TARGET) $(EXEC_DIR)/$(TARGET2) $(EXEC_DIR)/$(TARGET3_1) $(EXEC_DIR)/$(TARGET3_2_sender) $(EXEC_DIR)/$(TARGET3_2_receiver)

# Ziel 1
$(EXEC_DIR)/$(TARGET): $(OBJ) $(COMMON_OBJ)
	mkdir -p $(EXEC_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# Ziel 2
$(EXEC_DIR)/$(TARGET2): $(OBJ2) $(COMMON_OBJ)
	mkdir -p $(EXEC_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# Ziel 3_1
$(EXEC_DIR)/$(TARGET3_1): $(OBJ3_1) $(COMMON_OBJ)
	mkdir -p $(EXEC_DIR)
	$(CC) -o $@ $^ $(CFLAGS) -lzmq

# Ziel 3_2 (Sender)
$(EXEC_DIR)/$(TARGET3_2_sender): $(OBJ3_2_sender) $(COMMON_OBJ)
	mkdir -p $(EXEC_DIR)
	$(CC) -o $@ $^ $(CFLAGS) -lzmq

# Ziel 3_2 (Receiver)
$(EXEC_DIR)/$(TARGET3_2_receiver): $(OBJ3_2_receiver) $(COMMON_OBJ)
	mkdir -p $(EXEC_DIR)
	$(CC) -o $@ $^ $(CFLAGS) -lzmq

# Regel für .o-Dateien
$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

# Bereinigen
clean:
	rm -rf $(BUILD_DIR)
