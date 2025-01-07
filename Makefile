CC = gcc
CFLAGS = -Wall -g -Iinclude

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

MAIN_TARGET = $(BIN_DIR)/main

SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

MAIN_OBJ = $(SRC_DIR)/main.o

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(OBJ_FILES) $(MAIN_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

clean:
	rm -rf $(BIN_DIR)   
	rm -f $(SRC_DIR)/*.o
