#ifndef SERVER_FUNCS__
#define SERVER_FUNCS__

#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "util/game_values.h"

#define MAX_USERS 512
#define MAX_ROOMS 64
#define MAX_PARTIES MAX_USERS/MAX_PARTY_SIZE

void
id_pools_init(void);

void
id_pools_destroy(void);

uint16_t*
id_pool_party(void);

uint16_t*
id_pool_room(void);

bool
account_is_logged_in(uint32_t account_id);

uint32_t
hash_string(char *value);

bool
is_valid_email(char *email);

long long int
get_time_ms(void);

#endif



