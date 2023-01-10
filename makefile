default: all

all: common client server 

common: ./src/common.c
	gcc -c ./src/common.c -o common

client: ./src/client.c
	gcc -pthread ./src/client.c common -o client

server: ./src/server.c
	gcc ./src/server.c common -o server

clean:
	rm -f client server common