#include <string.h>
#include <roothandler.h>
#include <microserver.h>
#include <responseutil.h>

int RH_HandleGet(struct MHD_Connection *connection)
{
	char content[] = "<html><head><title>YOOO</title></head><body><h1>Hello World</h1></body>";
	int ret = micro_respond(connection, content, MHD_HTTP_OK, CONTENT_TYPE_HTML);
	return ret;
}
