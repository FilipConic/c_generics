CC = gcc
FLAGS = -Wall -Wextra -ggdb
TARGET = main

SRCS = main.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm $(OBJS) $(TARGET)
