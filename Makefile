int_max: int_max.o
	gcc int_max.o -o int_max -lncurses

int_max.o: int_max.c
	gcc -c int_max.c -lncurses

all: int_max

run: int_max
	./int_max

clean:
	rm -f int_max.o int_max
