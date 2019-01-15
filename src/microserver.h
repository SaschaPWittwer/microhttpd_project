#ifndef MICROSERVER_H
#define MICROSERVER_H

#define CONTENT_TYPE_HTML "text/html"
#define CONTENT_TYPE_JSON "application/json"

#endif

typedef struct PostHandle
{
    char *data;
    int len;
    int uid;
} PostHandle;