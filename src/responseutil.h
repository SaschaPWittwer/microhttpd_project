#include <microhttpd.h>

int micro_respond_JSON(struct MHD_Connection *connection, const char *json, unsigned int http_status_code);
