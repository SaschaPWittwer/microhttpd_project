#include <string.h>
#include <jansson.h>
#include <userhandler.h>
#include <responseutil.h>

int UH_HandleRequest(PGconn *db_conn, struct MHD_Connection *connection, const char *method)
{
	int ret = 0;
	if (strcmp(method, MHD_HTTP_METHOD_GET) == 0)
	{
		User *db_user = get_user(db_conn, "cevo", "super1234!");

		json_t *json = json_pack("{s:s, s:s}", "Username", db_user->username, "Password", db_user->password);
		char *content = json_dumps(json, 0);
		printf("Jason: %s\n", content);

		ret = micro_respond_JSON(connection, content, MHD_HTTP_OK);

		free((void*)db_user->username);
		free((void*)db_user->password);
		free(db_user);
		free(content);
		json_decref(json);
	}
	return ret;
}
