#include <jwt.h>
#include <microhttpd.h>
#include <string.h>

int validateJwt(struct MHD_Connection *connection);

char *generateJwt();