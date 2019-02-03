#include <string.h>
#include <jansson.h>
#include <userhandler.h>
#include <responseutil.h>
#include <microserver.h>

int UH_HandleGet(PGconn *db_conn, struct MHD_Connection *connection, const char *method, char *userId)
{
	int ret = 0;
	char *username = get_userById(db_conn, userId);

	json_t *json = json_pack("{s:s}", "Username", username);
	char *content = json_dumps(json, 0);

	if (username == NULL)
	{
		ret = micro_empty_response(connection, MHD_HTTP_NOT_FOUND);
	}
	else
	{
		ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_JSON);
	}

	json_decref(json);

	return ret;
}

int UH_HandleDelete(PGconn *db_conn, struct MHD_Connection *connection, const char *method, char *userId)
{
	int success = deleteUserById(db_conn, userId);
	if (success < 1)
	{
		return micro_empty_response(connection, MHD_HTTP_NOT_FOUND);
	}
	else
	{
		return micro_empty_response(connection, MHD_HTTP_OK);
	}
}

int UH_HandlePost(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
				  size_t *upload_data_size)
{
	int ret = 0;

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

	int res = create_user(db_conn, user);
	if (res < 1)
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

int UH_HandlePut(PGconn *db_conn, struct MHD_Connection *connection, void **con_cls, const char *method, const char *upload_data,
				 size_t *upload_data_size)
{
	int ret = 0;

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
	printf(user->username);

	int res = update_user(db_conn, user, meta->userId);
	if (res < 1)
		return micro_empty_response(connection, MHD_HTTP_NOT_FOUND);

	json_t *json = json_pack("{s:s}", "username", user->username);
	char *content = json_dumps(json, 0);
	ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_JSON);

	free(content);
	json_decref(json);
	json_decref(body);
	free(user);

	return ret;
}
