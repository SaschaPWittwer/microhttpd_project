#include <string.h>
#include <jansson.h>
#include <tokenhandler.h>
#include <responseutil.h>
#include <microserver.h>
#include <dbutil.h>
#include <microhttpd.h>
#include <auth.h>

int TH_HandleRequest(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
                     size_t *upload_data_size)
{
    int ret = 0;
    if (strcmp(method, MHD_HTTP_METHOD_POST) == 0)
    {
        if (NULL == *con_cls)
        {
            *con_cls = connection;
            return MHD_YES;
        }
        const char *param;
        const char *encoding;
        int contentlen = -1;
        PostHandle *posthandle = *con_cls;

        // Let make sure we have the right encoding and a valid length
        encoding = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_TYPE);
        param = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_LENGTH);
        if (param)
        {
            sscanf(param, "%i", &contentlen);
        }

        // In POST mode 1st call is only design to establish POST processor.
        // As JSON content is not supported out of box, but must provide something equivalent.
        if (posthandle == NULL)
        {
            posthandle = malloc(sizeof(PostHandle));   // allocate application POST processor handle           // build a UID for DEBUG
            posthandle->len = 0;                       // effective length within POST handler
            posthandle->data = malloc(contentlen + 1); // allocate memory for full POST data + 1 for '\0' enf of string
            *con_cls = posthandle;                     // attache POST handle to current HTTP session
            return MHD_YES;
        }

        // This time we receive partial/all Post data. Note that even if we get all POST data. We should nevertheless
        // return MHD_YES and not process the request directly. Otherwise Libmicrohttpd is unhappy and fails with
        // 'Internal application error, closing connection'.
        if (*upload_data_size)
        {
            memcpy(&posthandle->data[posthandle->len], upload_data, *upload_data_size);
            posthandle->len = posthandle->len + *upload_data_size;
            *upload_data_size = 0;
            return MHD_YES;
        }

        // We should only start to process DATA after Libmicrohttpd call or application handler with *upload_data_size==0
        // At this level we're may verify that we got everything and process DATA
        posthandle->data[posthandle->len] = '\0';

        // proceed request data
        json_t *body = json_loads(posthandle->data, posthandle->len, NULL);
        User *user = malloc(sizeof(User));
        user->username = json_string_value(json_object_get(body, "username"));
        user->password = json_string_value(json_object_get(body, "password"));

        User *db_user = get_user(db_conn, user->username, user->password);
        if (strcmp(db_user->username, user->username) == 0 && strcmp(db_user->password, user->password) == 0)
        {
            update_user(db_conn, user);
            char *token = generateJwt(db_user->username, "Admin");
            json_t *json = json_pack("{s:s}", "token", token);
            char *content = json_dumps(json, 0);
            ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_JSON);
            free(content);
            json_decref(json);
        }
        else
        {
            json_t *msg = json_pack("{s:s}", "msg", "Wrong Credentials");
            char *msgcontent = json_dumps(msg, 0);
            ret = micro_respond(connection, msgcontent, MHD_HTTP_UNAUTHORIZED, CONTENT_TYPE_JSON);
            free(msgcontent);
            json_decref(msg);
        }

        free((void *)db_user->username);
        free((void *)db_user->password);
        free(db_user);
    }
    return ret;
}
