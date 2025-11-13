#
# A simple Makefile for the Pixel Art Tool project.
#

# --- Project Details ---
PROJECT_NAME = pixel_art_tool
TARGET = $(PROJECT_NAME).exe

# --- Paths ---
# Location of the raylib library.
# IMPORTANT: You may need to change this to match your system.
RAYLIB_PATH = C:/raylib/raylib

# --- Compiler and Flags ---
CC = gcc
# CFLAGS: Compiler flags for C files
# LDFLAGS: Linker flags
# LDLIBS: Libraries to link
CFLAGS = -Wall -Wextra -g -O1 -Iinclude -I$(RAYLIB_PATH)/src
LDFLAGS = -L$(RAYLIB_PATH)/src
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# --- Source Files ---
SRCS = src/main.c src/canvas.c src/camera.c
OBJS = src/main.o src/canvas.o src/camera.o

# --- Build Rules ---

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

# Compile .c files
src/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/canvas.o: src/canvas.c
	$(CC) $(CFLAGS) -c src/canvas.c -o src/canvas.o

src/camera.o: src/camera.c
	$(CC) $(CFLAGS) -c src/camera.c -o src/camera.o

# --- Housekeeping ---

# Clean the build artifacts
clean:
	del /F /Q $(TARGET) src\*.o 2>nul

# --- Help ---
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"