all:
	gcc -Wall -c game.c
	gcc -Wall -c common.c
	gcc -Wall client.c common.o -o client
	gcc -Wall server.c common.o game.o -o server

clean:
	rm common.o game.o client server
