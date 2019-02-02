#include <stdio.h>
#include <microhttpd.h>

#include <microserver.h>
#include <dbutil.h>
#include <auth.h>
#include <roothandler.h>
#include <userhandler.h>
#include <tokenhandler.h>

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
	if (strncmp(url, "/user", 5) == 0)
	{
		int i = 0;
		char *p = strtok(url, "/");
		char *urlBits[2];

		while (p != NULL)
		{
			urlBits[i++] = p;
			p = strtok(NULL, "/");
		}
		int userId = urlBits[1];

		printf("Url: %s\n | Method: %s\n", url, method);
		if (strcmp(method, MHD_HTTP_METHOD_GET) == 0)
			return UH_HandleGet(db_conn, connection, method, userId);
		if (strcmp(method, MHD_HTTP_METHOD_POST) == 0)
			return UH_HandlePost(db_conn, connection, con_cls, method, upload_data, upload_data_size);
	}
	if (strcmp(url, "/token") == 0)
	{
		return TH_HandleRequest(db_conn, connection, con_cls, method, upload_data, upload_data_size);
	}

	return ret;
}

int main()
{
	db_conn = init_db_connection();
	init_db(db_conn);

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
