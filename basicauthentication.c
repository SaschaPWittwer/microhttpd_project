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

#include <libpq-fe.h>
#include <gnunet/platform.h>
#include <gnunet/gnunet_pq_lib.h>

#define PORT 8888

PGconn *conn;

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
  int fail;
  int ret;
  struct MHD_Response *response;

  // only allow GET and PUT
  if ((0 != strcmp (method, "GET")) && (0 != strcmp (method, "PUT")))
    return MHD_NO;
  if (NULL == *con_cls)
    {
      *con_cls = connection;
      return MHD_YES;
    }

  // check correct credentials
  fail = boys_check_auth(connection, "root", "pa$$w0rd");

  if (fail)
    {
      const char *page = "<html><body>Go away.</body></html>";
      response =
	MHD_create_response_from_buffer (strlen (page), (void *) page,
					 MHD_RESPMEM_PERSISTENT);
      ret = MHD_queue_basic_auth_fail_response (connection,
						"my realm",
						response);
    }
  else
    {
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



		const char *page = "<html><body>GET: A secret.</body></html>";
      		response =
			MHD_create_response_from_buffer (strlen (page), (void *) page, MHD_RESPMEM_PERSISTENT);
      		ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	}

	// handle PUT request
	if (0 == strcmp (method, "PUT")) {
		const char *page = "<html><body>PUT: A secret.</body></html>";
      		response =
			MHD_create_response_from_buffer (strlen (page), (void *) page, MHD_RESPMEM_PERSISTENT);
      		ret = MHD_queue_response (connection, MHD_HTTP_OK, response);	
	}
    }
  MHD_destroy_response (response);
  return ret;
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
