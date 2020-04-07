CC = gcc
CPPFLAGS = -O3
CFLAGS =
DEPS =
SRC = diam.c

diam: $(SRC)
	$(CC) $(CPPFLAGS) $^ -o $@ $(CFLAGS)

clean:
	rm -f diam *.o