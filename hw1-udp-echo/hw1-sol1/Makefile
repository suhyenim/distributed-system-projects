all: client server

client.out : client.o
	gcc -o client client.o

client.o : client.c
	gcc -c -o client.o client.c

server.out : server.o
	gcc -o server server.o

server.o : server.c
	gcc -c -o server.o server.c

clean:
	rm *.o client server
