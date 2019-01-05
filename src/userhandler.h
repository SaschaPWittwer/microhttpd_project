#include <dbutil.h>
#include <microhttpd.h>

int UH_HandleRequest(PGconn *db_conn, struct MHD_Connection *connection, const char *method);
