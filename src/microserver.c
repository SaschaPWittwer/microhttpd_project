#include <stdio.h>
#include <microhttpd.h>

#include <dbutil.h>

#define PORT 8888

PGconn *db_conn;

static int requestDispatcher(void *cls, struct MHD_Connection *connection, const char *url,
		const char *method, const char *version, const char *upload_data,
		size_t *upload_data_size, void **con_cls)
{
	int ret = 0;
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
	return ret;
}

int main()
{
	db_conn = init_db_connection();

	struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
			&requestDispatcher, NULL, MHD_OPTION_END);

	if (NULL == daemon)
	{
		return 1;
	}

	(void)getchar();

	MHD_stop_daemon(daemon);
	PQfinish(db_conn);
	return 0;
}
