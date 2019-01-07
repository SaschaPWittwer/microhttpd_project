#include <microhttpd.h>

int micro_respond(struct MHD_Connection *connection, const char *json,
		unsigned int http_status_code, const char* content_type);
