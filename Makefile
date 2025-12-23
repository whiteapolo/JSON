all: main.c parser.c lexer.c print_json.c token.c zlib/libzatar.a
	cc main.c parser.c lexer.c print_json.c token.c zlib/libzatar.a -o exe -lm -Wextra -Wall -pedantic

zlib/libzatar.a:
	make -C zlib