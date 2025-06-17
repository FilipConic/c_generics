CC = gcc
FLAGS = -Wall -Wextra -ggdb -I./src/include
TARGET = main

SRCS1 = binary_heap.c hashfuncs.c dynamic_string.c regex.c
SRCS = $(SRCS1:%.c=src/libs/%.c)

TARGET_DIR = ./build
OBJS = $(SRCS:src/libs/%.c=$(TARGET_DIR)/%.o) $(TARGET_DIR)/main.o

all: $(TARGET_DIR) $(TARGET)

$(TARGET_DIR):
ifeq ($(wildcard $(TARGET_DIR)),)
	mkdir -p $(TARGET_DIR)
endif

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@

$(TARGET_DIR)/main.o: main.c
	$(CC) $(FLAGS) -c $< -o $@

$(TARGET_DIR)/%.o: src/libs/%.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -r $(TARGET_DIR) $(TARGET)
