CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic -O2
LDFLAGS = -lm

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)
EXEC = $(BIN_DIR)/stego

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(OBJS) $(EXEC)
	rm -rf $(BIN_DIR) 