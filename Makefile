all:
	make -C zlib
	cc -O3 src/main.c zlib/build/libzatar.a -o exe -lm -Wall -Wextra
