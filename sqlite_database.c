#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define println(...)     \
	printf(__VA_ARGS__); \
	printf("\n");

static sqlite3 *db = NULL;

int main(void)
{
	println("An introduction to sqlite databases in C");

	if (SQLITE_OK != sqlite3_open_v2("database_file", &db, SQLITE_OPEN_READWRITE, NULL))
	{
		perror("Failed to open sqlite3 database");
		return EXIT_FAILURE;
	}
}