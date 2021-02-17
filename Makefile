all: server client

server: ipc_server.c
	gcc -g -o $@ $^
	chmod +x $@

client: ipc_client.c
	gcc -g -o $@ $^
	chmod +x $@

clean:
	rm -rf server client

.phony: clean client server