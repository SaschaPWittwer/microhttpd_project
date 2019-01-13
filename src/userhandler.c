#include <string.h>
#include <jansson.h>
#include <userhandler.h>
#include <responseutil.h>
#include <microserver.h>

int UH_HandleGet(PGconn *db_conn, struct MHD_Connection *connection, const char *method)
{
	int ret = 0;
	User *db_user = get_user(db_conn, "cevo", "super1234!");

	json_t *json = json_pack("{s:s, s:s}", "Username", db_user->username, "Password", db_user->password);
	char *content = json_dumps(json, 0);
	printf("Jason: %s\n", content);

	ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_JSON);

	free((void *)db_user->username);
	free((void *)db_user->password);
	free(db_user);
	free(content);
	json_decref(json);

	return ret;
}

int UH_HandlePut(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
				 size_t *upload_data_size)
{
	int ret = 0;

	if (NULL == *con_cls)
	{
		*con_cls = connection;
		return MHD_YES;
	}
	int contentlen = -1;
	PutHandle *puthandle = *con_cls;

	// In POST mode 1st call is only design to establish POST processor.
	// As JSON content is not supported out of box, but must provide something equivalent.
	if (puthandle == NULL)
	{
		puthandle = malloc(sizeof(PutHandle));	// allocate application POST processor handle           // build a UID for DEBUG
		puthandle->len = 0;						  // effective length within POST handler
		puthandle->data = malloc(contentlen + 1); // allocate memory for full POST data + 1 for '\0' enf of string
		*con_cls = puthandle;					  // attache POST handle to current HTTP session
		return MHD_YES;
	}

	// This time we receive partial/all Post data. Note that even if we get all POST data. We should nevertheless
	// return MHD_YES and not process the request directly. Otherwise Libmicrohttpd is unhappy and fails with
	// 'Internal application error, closing connection'.
	if (*upload_data_size)
	{
		memcpy(&puthandle->data[puthandle->len], upload_data, *upload_data_size);
		puthandle->len = puthandle->len + *upload_data_size;
		*upload_data_size = 0;
		return MHD_YES;
	}

	// We should only start to process DATA after Libmicrohttpd call or application handler with *upload_data_size==0
	// At this level we're may verify that we got everything and process DATA
	puthandle->data[puthandle->len] = '\0';

	// proceed request data
	json_t *body = json_loads(puthandle->data, puthandle->len, NULL);
	User *user = malloc(sizeof(User));
	user->username = json_string_value(json_object_get(body, "username"));
	user->password = json_string_value(json_object_get(body, "password"));

	int res = create_user(db_conn, user);
	if (res != 0)
		return micro_empty_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR);

	json_t *json = json_pack("{s:s}", "username", user->username);
	char *content = json_dumps(json, 0);
	ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_JSON);
	free(content);
	json_decref(json);

	free((void *)user->username);
	free((void *)user->password);
	free(user);

	return ret;
}