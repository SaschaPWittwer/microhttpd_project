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

#include <libpq-fe.h>
#include <gnunet/platform.h>
#include <gnunet/gnunet_pq_lib.h>

#define MAXANSWERSIZE   512
#define PORT 8888

struct connection_info_struct
{
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};

PGconn *conn;

static int send_json (struct MHD_Connection *connection, const char *json) {
  int ret;
  struct MHD_Response *response;
  response = MHD_create_response_from_buffer (strlen (json), (void *) json, MHD_RESPMEM_PERSISTENT);

  if (!response) {
    return MHD_NO;
  }

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);
  return ret;
}

static int
iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
              const char *filename, const char *content_type,
              const char *transfer_encoding, const char *data, uint64_t off,
              size_t size)
{
  struct connection_info_struct *con_info = coninfo_cls;


  json_t *j = json_pack("{s:i,s:i}", "hello", 5, "world", 10);
  char *answerstring;
  answerstring = malloc (MAXANSWERSIZE);

  if (!answerstring) {
    return MHD_NO;
  }

  char *s = json_dumps(j, 0);

  snprintf (answerstring, MAXANSWERSIZE, s, data);
  con_info->answerstring = answerstring;

  return MHD_NO;
}

static int boys_check_auth(struct MHD_Connection *connection, const char *exp_user, const char *exp_password) {
	char *user;
	char *pass;
	int fail;

	pass = NULL;
	user = MHD_basic_auth_get_username_password (connection, &pass);

  	fail = ( (user == NULL) ||
	   (0 != strcmp (user, exp_user)) ||
	   (0 != strcmp (pass, exp_password) ) );


  	if (user != NULL) free (user);
  	if (pass != NULL) free (pass);

	return fail;
}


static void handle_result(void *cls, PGresult *result, unsigned int num_results){
  for (unsigned int i=0;i < num_results; i++){
    char *val; size_t val_size;
    char *val2; size_t val2_size;
    struct GNUNET_PQ_ResultSpec rs[]={
      GNUNET_PQ_result_spec_variable_size ("usename", &val, &val_size), 
      GNUNET_PQ_result_spec_variable_size("pid", &val2, &val2_size),
      GNUNET_PQ_result_spec_end
    };
    GNUNET_PQ_extract_result (result, rs, i);

    char *pid = PQgetvalue(result, i, 0);
    char *usename = PQgetvalue(result, i, 1);
    printf("pid=%s usename=%s\n", pid, usename);

    GNUNET_PQ_cleanup_result (rs);
  }
}





static int
answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **con_cls)
{
  int ret;
  int fail;

  // only allow GET and PUT
  if ((0 != strcmp (method, "GET")) && (0 != strcmp (method, "PUT")) && (0 != strcmp (method, "POST"))) {
    return MHD_NO;
  }
    
  if (NULL == *con_cls) {
    *con_cls = connection;
    return MHD_YES;
  }

  // check correct credentials
  fail = boys_check_auth(connection, "root", "pa$$w0rd");

  if (fail) {
      json_t *j = json_pack("{s:i}", "no_auth", 5);
      char *s = json_dumps(j, 0);
      return send_json(connection, s);
  } else {
	  // handle GET request
	  if (0 == strcmp (method, "GET")) {
      struct GNUNET_PQ_PreparedStatement ps[] = {
        GNUNET_PQ_make_prepare("select_stats", "SELECT pid, usename FROM pg_stat_activity;",0),
        GNUNET_PQ_PREPARED_STATEMENT_END		
      };

		  GNUNET_PQ_prepare_statements(conn, ps);

      struct GNUNET_PQ_QueryParam params [] = {
        GNUNET_PQ_query_param_end
      };

      GNUNET_PQ_eval_prepared_multi_select(conn,"select_stats",params, &handle_result,NULL);
      json_t *j = json_pack("{s:i}", "GET", 5);
      char *s = json_dumps(j, 0);
      return send_json(connection, s);
	  }

    // handle PUT request
    if (0 == strcmp (method, "POST")) {
      json_t *j = json_pack("{s:i}", "POST", 5);
      char *s = json_dumps(j, 0);
      return send_json(connection, s);
    }
  }
  json_t *j = json_pack("{s:i}", "ERROR", 5);
  char *s = json_dumps(j, 0);
  return send_json(connection, s);
}


int
main ()
{

  conn = GNUNET_PQ_connect("dbname=mhd");

  struct MHD_Daemon *daemon;

  daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             &answer_to_connection, NULL, MHD_OPTION_END);
  if (NULL == daemon)
    return 1;

  (void) getchar ();

  MHD_stop_daemon (daemon);

  PQfinish(conn);

  return 0;
}
