# Compiler
CC := gcc

# Compiler flags
CFLAGS := -Wall -Wextra -Wno-unused-parameter -std=c11 -pedantic-errors -g -fsanitize=address -pthread -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE

# Source files directory
SRC_DIR := src

# Binary output directory
BIN_DIR := ../..