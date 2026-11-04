CC := cc
CFLAGS := -Wall -Wextra -lm -O3

all: build

build:
	cc $(CFLAGS) src/main.c zlib/src/all.c -o exe

clean:
	make clean -C zlib
	rm -f exe

.PHONY: zlib all clean build
