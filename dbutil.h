#include <stdio.h>
#include <libpq-fe.h>
#include <gnunet/platform.h>
#include <gnunet/gnunet_pq_lib.h>

#define TRUE      1;
#define FALSE     0;

/**
 * struct to hold a users details
 */
typedef struct {
    char *username;
    char *password;
} User;

PGconn* init_db_connection();

/**
 * initialize the database scheme
 */
unsigned int init_db(PGconn* db_conn);

/**
 *  function to create a user
 */
unsigned int create_user(PGconn* db_conn, User* user);

/**
 * Get a user by username
 */
User* get_user(PGconn* db_conn, char* username, char* password);