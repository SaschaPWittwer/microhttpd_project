#include <microhttpd.h>

#ifndef MICROSERVER_H
#define MICROSERVER_H

#define CONTENT_TYPE_HTML "text/html"
#define CONTENT_TYPE_JSON "application/json"

#endif

typedef struct MySessionData
{
    char *userId;
    struct MHD_Connection *connection;
    int jwtValid;

    char *data;
    int len;
    int uid;

} MySessionData;