#include <stdio.h>
#include <microhttpd.h>

#include <microserver.h>
#include <dbutil.h>
#include <roothandler.h>
#include <userhandler.h>

#define PORT 8888

PGconn *db_conn;

static int requestDispatcher(void *cls, struct MHD_Connection *connection, const char *url,
		const char *method, const char *version, const char *upload_data,
		size_t *upload_data_size, void **con_cls)
{
	int ret = 0;
	if (NULL == *con_cls)
	{
		*con_cls = connection;
		return MHD_YES;
	}
	if (strcmp(url, "/") == 0 && strcmp(method, MHD_HTTP_METHOD_GET) == 0)
	{
		return RH_HandleGet(connection);
	}
	if (strcmp(url, "/user") == 0)
	{
		return UH_HandleRequest(db_conn, connection, method);
	}

	return ret;
}

int main()
{
	// GNUNET_log_setup("microserver", "DEBUG", NULL);
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
