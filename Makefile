all: main.c parser.c lexer.c print_json.c token.c
	cc main.c parser.c lexer.c print_json.c token.c -o exe -lm -Wextra -Wall -pedantic
