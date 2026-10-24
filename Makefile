CC := cc
CFLAGS := -Wall -Wextra -O3
DEP := $(wildcard src/*.c src/*.h)

all: exe

zlib:
	make -C zlib

exe: $(DEP) zlib
	cc $(CFLAGS) src/main.c zlib/build/libzatar.a -o exe -lm

clean:
	make clean -C zlib
	rm -f exe

.PHONY: zlib all clean
