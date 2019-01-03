/*
 * libpq test
 *
 * Compile with: gcc -g -Wall -lpq pqtest.c
 */
#include <stdio.h>
#include <stdlib.h>

#include "libpq-fe.h"

int main(int argc, char **argv) {
	PGconn     *conn;
	PGresult   *res;

	conn = PQconnectdb("dbname=djboris");

	if (PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		return 1;
	}

	res = PQexec(conn, "SELECT pid, datname, backend_start from pg_stat_activity");
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
		PQclear(res);

		PQfinish(conn);
		return 1;
	}

	int nrows = PQntuples(res);
	for(int i = 0; i < nrows; i++) {
		char *pid = PQgetvalue(res, i, 0);
		char *datname = PQgetvalue(res, i, PQfnumber(res, "datname"));
		char *bs = PQgetvalue(res, i, 2);

		printf("pid=%s datname=%s backend_start=%s\n", pid, datname, bs);
	}

	PQclear(res);
	PQfinish(conn);
	return 0;
}
