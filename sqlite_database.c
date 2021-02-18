#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define SQLITE_DB "database_file"

#define println(...)     \
	printf(__VA_ARGS__); \
	printf("\n");

static sqlite3 *db = NULL;

int main(void)
{
	println("An introduction to sqlite databases in C");

	if (SQLITE_OK != sqlite3_open_v2(SQLITE_DB, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))
	{
		perror("Failed to open sqlite3 database");
		return EXIT_FAILURE;
	}
}