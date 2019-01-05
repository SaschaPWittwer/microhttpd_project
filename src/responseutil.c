#include <string.h>
#include <microserver.h>
#include <responseutil.h>

int micro_respond_JSON(struct MHD_Connection *connection, const char *json, unsigned int http_status_code)
{
  int ret;
  struct MHD_Response *response;
  response = MHD_create_response_from_buffer(strlen(json), (void *)json, MHD_RESPMEM_PERSISTENT);
  if (!response)
  {
	  return MHD_NO;
  }

  MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, CONTENT_TYPE_JSON);

  ret = MHD_queue_response(connection, http_status_code, response);
  MHD_destroy_response(response);
  return ret;
}
