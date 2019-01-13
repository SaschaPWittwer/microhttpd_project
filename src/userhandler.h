#include <dbutil.h>
#include <microhttpd.h>

typedef struct PutHandle
{
    char *data;
    int len;
    int uid;
} PutHandle;

int UH_HandleGet(PGconn *db_conn, struct MHD_Connection *connection, const char *method);

int UH_HandlePut(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
                 size_t *upload_data_size);
