MAKEFLAGS += --no-print-directory
.SILENT:

# Compiler and flags
CC = gcc
CFLAGS = -l/usr/include/cjson -Wall -Wextra -g
LDFLAGS = -lcjson

# Source files
SRCS = lexer.c parser.c inter_rep.c codegen.c error_handler.c main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output executeable
TARGET = ease

# Default rule
all: $(TARGET)

# Linking the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile each .c file into .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Running the code
run:
	make
	./${TARGET} tests/test1.ease build/program
	make clean
	gcc build/program.c -o build/program
	./build/program
	make clean

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET) build/*

# Phony targets (not actual files)
.PHONY: all clean