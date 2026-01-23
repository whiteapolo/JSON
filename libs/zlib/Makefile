CC = cc
CFLAGS = -I./include -O3

all:
	@mkdir -p obj
	$(CC) $(CFLAGS) -c ./src/all.c -o obj/libzatar.o
	ar rcs libzatar.a ./obj/libzatar.o

clean:
	rm -rf obj libzatar.a

.PHONY: all clean
