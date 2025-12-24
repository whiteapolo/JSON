all: main.c parser.c lexer.c print_json.c token.c zlib/libzatar.a
	cc main.c parser.c lexer.c print_json.c token.c zlib/libzatar.a -o exe -lm -Wextra -Wall -pedantic -I./zlib/include -O3 -g

zlib/libzatar.a:
	make -C zlib
