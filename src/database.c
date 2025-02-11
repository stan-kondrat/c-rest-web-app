#include <stdio.h>
#include <stdlib.h>

#include <libpq-fe.h>

int database() {
    const char* conninfo =
        "dbname=postgres user=user password= hostaddr=127.0.0.1 port=5432";

    // Connect to the database
    PGconn* conn = PQconnectdb(conninfo);

    // Check if connection is successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    printf("Connected to the database successfully!\n");

    // Run a query
    PGresult* res = PQexec(conn, "SELECT VERSION();");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

    // Print the results
    int nrows = PQntuples(res);
    for (int i = 0; i < nrows; i++) {
        printf("Row %d: %s\n", i + 1,
               PQgetvalue(res, i, 0)); // Assuming a single column result
    }

    // Clean up
    PQclear(res);
    PQfinish(conn);

    return 0;
}
