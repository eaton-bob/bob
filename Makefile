all: helloworld_user.c helloworld_provider.c
	gcc -lmlm -lczmq -o helloworld_user helloworld_user.c
	gcc -lmlm -lczmq -o helloworld_provider helloworld_provider.c
