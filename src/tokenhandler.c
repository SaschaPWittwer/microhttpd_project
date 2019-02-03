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
        int contentlen = -1;
        MySessionData *meta = *con_cls;

        if (meta->data == NULL)
        {
            meta->len = 0;
            meta->data = malloc(contentlen + 1);
        }

        if (*upload_data_size)
        {
            meta->data = malloc((int)*upload_data_size + 1);
            memcpy(&meta->data[meta->len], upload_data, *upload_data_size);
            meta->len = meta->len + *upload_data_size;
            *upload_data_size = 0;
            return MHD_YES;
        }

        meta->data[meta->len] = '\0';

        json_t *body = json_loads(meta->data, meta->len, NULL);
        User *user = malloc(sizeof(User));
        user->username = json_string_value(json_object_get(body, "username"));
        user->password = json_string_value(json_object_get(body, "password"));

        User *db_user = get_user(db_conn, user->username, user->password);
        if (strcmp(db_user->username, user->username) == 0 && strcmp(db_user->password, user->password) == 0)
        {
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

        json_decref(body);
        free(db_user);
    }
    return ret;
}
