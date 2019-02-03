#include <dbutil.h>
#include <microhttpd.h>

int TH_HandleRequest(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
                     size_t *upload_data_size);