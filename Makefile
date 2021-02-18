all: server client

server: ipc_server.c
	gcc -g -o $@ $^
	chmod +x $@

client: ipc_client.c
	gcc -g -o $@ $^
	chmod +x $@

database: sqlite_database.c
	gcc -g -o $@ -lsqlite3 $^
	chmod +x $@

clean:
	rm -rf server client database*

.phony: clean client server