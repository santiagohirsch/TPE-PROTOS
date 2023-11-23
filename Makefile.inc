# Compiler
CC := gcc

# Compiler flags
# CFLAGS := -Wall -std=c11 -fsanitize=address
CFLAGS := -Wall -std=c11 -pthread -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE

# Source files directory
SRC_DIR := src

# Binary output directory
BIN_DIR := bin