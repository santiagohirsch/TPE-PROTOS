include Makefile.inc

# Source files
SRC := $(shell find . -name "*.c")

# Object files

OBJ := $(pathsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC))

# Binary output

TARGET := $(BIN_DIR)/main

# Default target

all: $(TARGET)

# Compile and link the source files

$(TARGET): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Clean the object files and the binary

clean:
	rm -rf $(BIN_DIR)