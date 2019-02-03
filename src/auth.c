#include <jwt.h>
#include <microhttpd.h>
#include <string.h>
#include <time.h>

static const unsigned char *key = (unsigned char *)"kjahsdfb72723ubasdfhjbhaskfj!!asd";

int validateJwt(struct MHD_Connection *connection)
{
    const char *authHeaderValue = MHD_lookup_connection_value(connection,
                                                              MHD_HEADER_KIND,
                                                              MHD_HTTP_HEADER_AUTHORIZATION);

    if (NULL == authHeaderValue)
        return 0;

    const char *type = strtok((char *)authHeaderValue, " ");
    const char *token = strtok(NULL, " ");

    printf("Type: %s\n", type);
    printf("Token: %s\n", token);

    jwt_t *jwt = NULL;
    int valid = jwt_decode(&jwt, token, key, strlen((char *)key));
    const char *iat = jwt_get_grant(jwt, "iat");
    printf("Issued at: %s\n", iat);

    return valid;
}

char *generateJwt(char *username, char *role)
{
    jwt_t *jwt = NULL;
    int success = jwt_new(&jwt);
    if (success != 0)
        return NULL;

    success = jwt_set_alg(jwt, JWT_ALG_HS256, key, strlen((char *)key));
    if (success != 0)
        return NULL;

    success = jwt_add_grant(jwt, "user", username);
    if (success != 0)
        return NULL;

    success = jwt_add_grant(jwt, "role", role);
    if (success != 0)
        return NULL;

    const time_t timestamp = time(NULL);
    success = jwt_add_grant(jwt, "iat", ctime(&timestamp));
    if (success != 0)
        return NULL;

    char *token = jwt_encode_str(jwt);
    jwt_free(jwt);
    return token;
}