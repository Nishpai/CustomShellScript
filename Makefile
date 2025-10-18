# Makefile for myShell - Custom Unix-like Shell
# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g -I./include
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = .

# Target executable
TARGET = $(BIN_DIR)/myShell

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Header files
HEADERS = $(wildcard $(INC_DIR)/*.h)

# Default target
all: $(TARGET)

# Create directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link object files to create executable
$(TARGET): $(OBJS) | $(OBJ_DIR)
	@echo "Linking: $@"
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Clean complete"

# Rebuild everything
rebuild: clean all

# Run the shell
run: $(TARGET)
	./$(TARGET)

# Debug build with additional flags
debug: CFLAGS += -DDEBUG=1 -O0
debug: rebuild

# Release build with optimizations
release: CFLAGS += -O2 -DNDEBUG
release: rebuild

# Check for memory leaks (requires valgrind)
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Display help
help:
	@echo "myShell Makefile"
	@echo "=================="
	@echo "Available targets:"
	@echo "  all       - Build the shell (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and rebuild"
	@echo "  run       - Build and run the shell"
	@echo "  debug     - Build with debug symbols and verbose output"
	@echo "  release   - Build optimized release version"
	@echo "  valgrind  - Run with valgrind memory checker"
	@echo "  help      - Display this help message"

# Phony targets
.PHONY: all clean rebuild run debug release valgrind help
