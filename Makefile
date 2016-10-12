all: bob.c
	gcc -lmlm -lczmq -o bob bob.c
