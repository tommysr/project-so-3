default: all

all: client server

client: ./src/client.c
	gcc ./src/client.c -o client


server: ./src/server.c
	gcc ./src/server.c -o server

clean:
	rm -f client server