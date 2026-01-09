all:
	make -C libs/zlib
	cc src/main.c libs/zlib/libzatar.a -o exe -lm -Wextra -Wall -pedantic -I./libs/zlib/include -g
	
