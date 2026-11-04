CC := cc
CFLAGS := -Wall -Wextra

all: build

build:
	cc $(CFLAGS) src/main.c zlib/src/all.c -o exe -lm

clean:
	make clean -C zlib
	rm -f exe

.PHONY: zlib all clean build
