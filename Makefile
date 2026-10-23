all:
	make dev -C zlib
	cc src/main.c zlib/build/libzatar.a -o exe -lm -g -Wall -Wextra
