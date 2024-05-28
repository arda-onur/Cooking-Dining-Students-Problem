# Arda Onur, S018752
CC=gcc
BINARY=kitchen

LIB=-pthread
INLCUDE=

%: %.c
	$(CC) -o $@ $< $(LIB)

all: $(BINARY)

clean:
	rm -f *.o $(BINARY)
