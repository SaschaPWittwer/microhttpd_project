#include <string.h>
#include <roothandler.h>

int RH_HandleGet(struct MHD_Connection *connection)
{
	int ret = 0;
	char content[] = "Hello World!";
	struct MHD_Response *response = MHD_create_response_from_buffer(strlen(content),
			(void*)content, MHD_RESPMEM_PERSISTENT);
	ret = MHD_queue_response(connection, 200, response);
	MHD_destroy_response(response);
	return ret;
}
