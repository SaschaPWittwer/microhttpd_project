/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <jansson.h>

#include <dbutil.h>

#define MAXANSWERSIZE   512
#define PORT 8888

#define JSON_CONTENT  "application/json"
#define MAX_POST_SIZE 4096
static int postcount = 0;

typedef struct PostHandle {
  char *data;
  int len;
  int uid;
} PostHandle;

PGconn *db_conn;

static int send_json (struct MHD_Connection *connection, const char *json, unsigned int http_status_code) {
  int ret;
  struct MHD_Response *response;
  response = MHD_create_response_from_buffer (strlen (json), (void *) json, MHD_RESPMEM_PERSISTENT);

  if (!response) {
    return MHD_NO;
  }

  ret = MHD_queue_response (connection, http_status_code, response);
  MHD_destroy_response (response);
  return ret;
}

static int boys_check_auth(struct MHD_Connection *connection) {
	char *given_username;
	char *given_pass;

	given_pass = NULL;
	given_username = MHD_basic_auth_get_username_password (connection, &given_pass);
  User* db_user = get_user(db_conn, given_username, given_pass);

  if (db_user != NULL) {
    free (given_username);
  } 

  if (db_user != NULL) {
    free (given_pass);
  }

  return db_user == NULL;
}

static int answer_to_connection (void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
  int fail;

  // only allow GET and PUT
  if ((0 != strcmp (method, "GET")) && (0 != strcmp (method, "PUT")) && (0 != strcmp (method, "POST"))) {
    return MHD_NO;
  }

  // check correct credentials
  fail = boys_check_auth(connection);

  if (fail) {
      json_t *j = json_pack("{s:i}", "no_auth", 5);
      char *s = json_dumps(j, 0);
      return send_json(connection, s, MHD_HTTP_UNAUTHORIZED);
  } else {
	  // handle GET request
	  if (0 == strcmp (method, "GET")) {
      json_t *j = json_pack("{s:i}", "GET", 5);
      char *s = json_dumps(j, 0);
      return send_json(connection, s, MHD_HTTP_OK);
	  }

    // handle POST request
    if (0 == strcmp (method, "POST")) {
      const char *param;
      const char *encoding;
      int    contentlen=-1;
      PostHandle *posthandle = *con_cls;

      // Let make sure we have the right encoding and a valid length
      encoding = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_TYPE);
      param = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_LENGTH);
      if (param) {
        sscanf (param,"%i",&contentlen);
      } 

      // In POST mode 1st call is only design to establish POST processor.
      // As JSON content is not supported out of box, but must provide something equivalent.
      if (posthandle == NULL) {
        posthandle = malloc (sizeof (PostHandle));      // allocate application POST processor handle
        posthandle->uid = postcount ++;                 // build a UID for DEBUG
        posthandle->len = 0;                            // effective length within POST handler
        posthandle->data= malloc (contentlen +1);       // allocate memory for full POST data + 1 for '\0' enf of string
        *con_cls = posthandle;                          // attache POST handle to current HTTP session
        return MHD_YES;
      }

      // This time we receive partial/all Post data. Note that even if we get all POST data. We should nevertheless
      // return MHD_YES and not process the request directly. Otherwise Libmicrohttpd is unhappy and fails with
      // 'Internal application error, closing connection'.
      if (*upload_data_size) {
        fprintf (stderr, "Update Post Request UID=%d\n", posthandle->uid);
        memcpy(&posthandle->data[posthandle->len], upload_data, *upload_data_size);
        posthandle->len = posthandle->len + *upload_data_size;
        *upload_data_size = 0;
        return MHD_YES;
      }

      // We should only start to process DATA after Libmicrohttpd call or application handler with *upload_data_size==0
      // At this level we're may verify that we got everything and process DATA
      posthandle->data[posthandle->len] = '\0';

      // proceed request data
      json_t *body = json_loads(posthandle->data, posthandle->len, NULL);
      User* user = malloc(sizeof(User));
      user->username = json_string_value(json_object_get(body, "username"));
      user->password = json_string_value(json_object_get(body, "password"));
      create_user(db_conn, user);

      json_t *j = json_pack("{s:i}", "POST", 5);
      char *s = json_dumps(j, 0);
      return send_json(connection, s, MHD_HTTP_OK);
    }
  }

  json_t *j = json_pack("{s:i}", "ERROR", 5);
  char *s = json_dumps(j, 0);
  return send_json(connection, s, MHD_HTTP_INTERNAL_SERVER_ERROR);
}

int main () {
  db_conn = init_db_connection();
  int success = init_db(db_conn);

  struct MHD_Daemon *daemon;
  daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);

  if (NULL == daemon) {
    return 1;
  }

  (void) getchar ();

  MHD_stop_daemon (daemon);
  PQfinish(db_conn);
  return 0;
}
