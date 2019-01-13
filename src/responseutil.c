#include <string.h>
#include <microserver.h>
#include <responseutil.h>

int micro_respond(struct MHD_Connection *connection, const char *json,
                  unsigned int http_status_code, const char *content_type)
{
  int ret;
  struct MHD_Response *response;
  response = MHD_create_response_from_buffer(strlen(json), (void *)json, MHD_RESPMEM_MUST_COPY);
  if (!response)
  {
    return MHD_NO;
  }

  MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);

  ret = MHD_queue_response(connection, http_status_code, response);
  MHD_destroy_response(response);
  return ret;
}

int micro_empty_response(struct MHD_Connection *connection, unsigned int http_status_code)
{
  // struct MHD_Response *response;
  // response = MHD_create_response_from_buffer(0, (void *)NULL, MHD_RESPMEM_MUST_FREE);
  int ret = MHD_queue_response(connection, http_status_code, NULL);
  return ret;
}