#include <dbutil.h>
#include <microhttpd.h>

int UH_HandleGet(PGconn *db_conn, struct MHD_Connection *connection, const char *method, char *userId);

int UH_HandlePost(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
                  size_t *upload_data_size);

int UH_HandlePut(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
                 size_t *upload_data_size);

int UH_HandleDelete(PGconn *db_conn, struct MHD_Connection *connection, const char *method, char *userId);