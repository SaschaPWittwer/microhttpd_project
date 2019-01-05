#include <string.h>
#include <userhandler.h>

int UH_HandleRequest(PGconn *db_conn, struct MHD_Connection *connection, const char *method)
{
	int ret = 0;
	if (strcmp(method, MHD_HTTP_METHOD_GET) == 0)
	{
		User *db_user = get_user(db_conn, "cevo", "super1234!");
		int len = strlen(db_user->username) + strlen(db_user->password) + 12;
		char *output = malloc(len*sizeof(char)+1);
		sprintf(output, "User: %s, Pw: %s", db_user->username, db_user->password);
		printf("User: %s, Pw: %s", db_user->username, db_user->password);

		struct MHD_Response *response = MHD_create_response_from_buffer(strlen(output),
				(void*)output, MHD_RESPMEM_PERSISTENT);
		if (!response)
		{
			return MHD_NO;
		}

		ret = MHD_queue_response(connection, 200, response);
		free((void*)db_user->username);
		free((void*)db_user->password);
		free(db_user);
		free(output);
		MHD_destroy_response(response);
	}
	return ret;
}
