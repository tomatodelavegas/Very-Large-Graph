CC = gcc
CFLAGS = -O3 -Iinclude
CGDBFLAGS = -O3 -Iinclude -g -DTEST -D_GNU_SOURCE
BIN = diam
SRCS = $(wildcard src/*.c)
OBJS = ${SRCS:.c=.o}

all:
	$(CC) $(SRCS) $(CFLAGS) -o $(BIN)

test: clean
	$(CC) $(SRCS) $(CGDBFLAGS) -o $(BIN)

clean:
	rm -f $(BIN) $(OBJS)