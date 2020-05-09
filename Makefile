CC = gcc
CFLAGS = -O3 -Iinclude
CGDBFLAGS = $(CFLAGS) -g
BIN = diam
SRCS = $(wildcard src/*.c)
OBJS = ${SRCS:.c=.o}

all:
	$(CC) $(SRCS) $(CFLAGS) -o $(BIN)

gdb: clean
	$(CC) $(SRCS) $(CGDBFLAGS) -o $(BIN)

clean:
	rm -f $(BIN) $(OBJS)