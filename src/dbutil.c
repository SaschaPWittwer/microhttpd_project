#include "dbutil.h"

PGconn *init_db_connection()
{
	return GNUNET_PQ_connect("dbname=mhd");
}

/**
 * initialize the database scheme
 */
unsigned int init_db(PGconn *db_conn)
{
	struct GNUNET_PQ_ExecuteStatement s[] = {
		GNUNET_PQ_make_execute("CREATE TABLE IF NOT EXISTS \"user\" (username VARCHAR(69) NOT NULL PRIMARY KEY, password VARCHAR NOT NULL);\n"),
		GNUNET_PQ_EXECUTE_STATEMENT_END};

	int success = GNUNET_PQ_exec_statements(db_conn, s);

	if (success <= 0)
	{
		printf("init_db failed");
		return FALSE;
	}

	printf("init_db succeeded");
	return TRUE;
}

unsigned int create_user(PGconn *db_conn, User *user)
{
	struct GNUNET_PQ_PreparedStatement s[] = {
		GNUNET_PQ_make_prepare("insert_user", "INSERT INTO \"user\" (username, password) VALUES ($1, $2);\n", 2),
		GNUNET_PQ_PREPARED_STATEMENT_END};
	GNUNET_PQ_prepare_statements(db_conn, s);

	struct GNUNET_PQ_QueryParam params[] = {
		GNUNET_PQ_query_param_string(user->username),
		GNUNET_PQ_query_param_string(user->password),
		GNUNET_PQ_query_param_end};

	int success = GNUNET_PQ_eval_prepared_non_select(db_conn, "insert_user", params);
	return TRUE;
}

unsigned int update_user(PGconn *db_conn, User *user)
{
	struct GNUNET_PQ_PreparedStatement s[] = {
		GNUNET_PQ_make_prepare("update_user", "UPDATE \"user\" SET password = $2 WHERE username = $1;\n", 2),
		GNUNET_PQ_PREPARED_STATEMENT_END};
	GNUNET_PQ_prepare_statements(db_conn, s);

	struct GNUNET_PQ_QueryParam params[] = {
		GNUNET_PQ_query_param_string(user->username),
		GNUNET_PQ_query_param_string(user->password),
		GNUNET_PQ_query_param_end};

	int success = GNUNET_PQ_eval_prepared_non_select(db_conn, "update_user", params);
	return TRUE;
}

User *get_user(PGconn *db_conn, const char *username, const char *password)
{
	struct GNUNET_PQ_PreparedStatement s[] = {
		GNUNET_PQ_make_prepare("select_user", "SELECT username, password FROM \"user\" WHERE username=$1 AND password=$2;", 2),
		GNUNET_PQ_PREPARED_STATEMENT_END};
	GNUNET_PQ_prepare_statements(db_conn, s);

	struct GNUNET_PQ_QueryParam params[] = {
		GNUNET_PQ_query_param_string(username),
		GNUNET_PQ_query_param_string(password),
		GNUNET_PQ_query_param_end};

	char *val_username;
	size_t username_size;
	char *val_password;
	size_t password_size;

	struct GNUNET_PQ_ResultSpec rs[] = {
		GNUNET_PQ_result_spec_variable_size("username", (void **)&val_username, &username_size),
		GNUNET_PQ_result_spec_variable_size("password", (void **)&val_password, &password_size),
		GNUNET_PQ_result_spec_end};

	int result = GNUNET_PQ_eval_prepared_singleton_select(db_conn, "select_user", params, rs);

	if (result == GNUNET_DB_STATUS_SUCCESS_ONE_RESULT)
	{
		User *user = malloc(sizeof(User));
		if (user != NULL)
		{
			user->username = val_username;
			user->password = val_password;
			return user;
		}
	}

	return NULL;
}
