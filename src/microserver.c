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

static int
requestDispatcher(void *cls, struct MHD_Connection *connection, const char *url,
				  const char *method, const char *version, const char *upload_data,
				  size_t *upload_data_size, void **con_cls)
{
	printf("Start handling request");

	int ret = 0;

	MySessionData *data = *con_cls;
	if (NULL == data)
	{
		data = malloc(sizeof(MySessionData));
		data->connection = connection;
		data->userId = NULL;
		data->jwtValid = 0;
		data->data = NULL;
		data->len = 0;
		data->uid = 0;
		*con_cls = data;
		return MHD_YES;
	}

	if (0 == data->jwtValid)
	{
		data->jwtValid = validateJwt(connection);
	}

	if (strcmp(url, "/") == 0 && strcmp(method, MHD_HTTP_METHOD_GET) == 0)
	{
		return RH_HandleGet(connection);
	}
	if (strncmp(url, "/user", 5) == 0)
	{
		if (strcmp(method, MHD_HTTP_METHOD_GET) == 0)
		{
			if (NULL == data->userId)
			{
				char *urlCopy = malloc(sizeof(7));
				strcpy(urlCopy, url);

				int i = 0;
				char *p = strtok(urlCopy, "/");
				char *urlBits[2];

				while (p != NULL)
				{
					urlBits[i++] = p;
					p = strtok(NULL, "/");
				}
				data->userId = urlBits[1];
			}
			return UH_HandleGet(db_conn, connection, method, data->userId);
		}

		if (strcmp(method, MHD_HTTP_METHOD_DELETE) == 0)
		{
			if (NULL == data->userId)
			{
				char *urlCopy = malloc(sizeof(7));
				strcpy(urlCopy, url);

				int i = 0;
				char *p = strtok(urlCopy, "/");
				char *urlBits[2];

				while (p != NULL)
				{
					urlBits[i++] = p;
					p = strtok(NULL, "/");
				}
				data->userId = urlBits[1];
			}
			return UH_HandleDelete(db_conn, connection, method, data->userId);
		}

		if (strcmp(method, MHD_HTTP_METHOD_POST) == 0)
			return UH_HandlePost(db_conn, connection, con_cls, method, upload_data, upload_data_size);
		if (strcmp(method, MHD_HTTP_METHOD_PUT) == 0)
		{
			if (NULL == data->userId)
			{
				char *urlCopy = malloc(sizeof(7));
				strcpy(urlCopy, url);

				int i = 0;
				char *p = strtok(urlCopy, "/");
				char *urlBits[2];

				while (p != NULL)
				{
					urlBits[i++] = p;
					p = strtok(NULL, "/");
				}
				data->userId = urlBits[1];
			}
			return UH_HandlePut(db_conn, connection, con_cls, method, upload_data, upload_data_size);
		}
	}
	if (strcmp(url, "/token") == 0)
	{
		return TH_HandleRequest(db_conn, connection, con_cls, method, upload_data, upload_data_size);
	}

	free(data);
	return ret;
}

int main()
{
	// GNUNET_log_setup("microserver", "DEBUG", NULL);
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
