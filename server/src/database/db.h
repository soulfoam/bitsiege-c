#ifndef DB__
#define DB__

#include <inttypes.h>
#include <mysql.h>
#include <stdbool.h>
#include "../network/client.h"

struct
database
{
	MYSQL *con;
};

struct database*
db(void);

uint8_t
db_init(void);

void
db_destroy(void);

bool
db_character_exists(const char *name);

bool
db_account_exists(const char *username);

uint32_t
db_get_account_id(char *username);

uint16_t
db_account_register(char *username, const char *password, char *email, uint32_t ip);

uint16_t
db_account_login(struct client *cl, char *username, const char *password);

struct friend_list
db_account_friend_list_get(uint32_t db_id, int8_t *result);

#endif

