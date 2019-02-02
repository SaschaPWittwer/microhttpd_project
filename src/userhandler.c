#include <string.h>
#include <jansson.h>
#include <userhandler.h>
#include <responseutil.h>
#include <microserver.h>

int UH_HandleGet(PGconn *db_conn, struct MHD_Connection *connection, const char *method, int userId)
{
	int ret = 0;
	User *db_user = get_userById(userId);

	json_t *json = json_pack("{s:s}", "Username", db_user->username);
	char *content = json_dumps(json, 0);
	printf("Jason: %s\n", content);

	ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_JSON);

	free((void *)db_user->username);
	free(db_user);
	free(content);
	json_decref(json);

	return ret;
}

int UH_HandlePost(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
				  size_t *upload_data_size)
{
	int ret = 0;

	int contentlen = -1;
	PostHandle *posthandle = *con_cls;

	// In POST mode 1st call is only design to establish POST processor.
	// As JSON content is not supported out of box, but must provide something equivalent.
	if (posthandle == NULL)
	{
		posthandle = malloc(sizeof(PostHandle));   // allocate application POST processor handle           // build a UID for DEBUG
		posthandle->len = 0;					   // effective length within POST handler
		posthandle->data = malloc(contentlen + 1); // allocate memory for full POST data + 1 for '\0' enf of string
		*con_cls = posthandle;					   // attache POST handle to current HTTP session
		return MHD_YES;
	}

	// This time we receive partial/all Post data. Note that even if we get all POST data. We should nevertheless
	// return MHD_YES and not process the request directly. Otherwise Libmicrohttpd is unhappy and fails with
	// 'Internal application error, closing connection'.
	if (*upload_data_size)
	{
		posthandle->data = malloc((int)*upload_data_size + 1);
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

	int res = create_user(db_conn, user);
	if (res != 1)
		return micro_empty_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR);

	json_t *json = json_pack("{s:s}", "username", user->username);
	char *content = json_dumps(json, 0);
	ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_JSON);

	free(content);
	json_decref(json);
	json_decref(body);
	free(user);

	return ret;
}

