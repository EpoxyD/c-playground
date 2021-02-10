all: server client

server:
	gcc -g -o server ipc_server.c

client:
	gcc -g -o client ipc_client.c

clean:
	rm -rf server client

.phony: clean