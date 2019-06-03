#include <regex.h>
#include <time.h>
#include <sys/timeb.h>
#include "server_funcs.h"
#include "server.h"
#include "util/id_pool.h"
#include "util/log.h"

static uint16_t *pool_party;
static uint16_t *pool_room;

void
id_pools_init(void)
{
	pool_party = id_pool_init(MAX_USERS);
	pool_room = id_pool_init(MAX_ROOMS);

	log_info("ID pools initialized.");
}

void
id_pools_destroy(void)
{
	free(pool_party);
	free(pool_room);

	log_info("ID pools freed.");
}

uint16_t*
id_pool_party(void)
{
	return pool_party;	
}

uint16_t*
id_pool_room(void)
{
	return pool_room;
}


bool
account_is_logged_in(uint32_t account_id)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		const struct client *c = &server()->clients[i];
		if (c->info.logged_in && c->info.id_account == account_id)
		{
			return true;
		}
	}

	return false;
}

uint32_t
hash_string(char *value)
{
	uint32_t len = strlen(value); 
	uint32_t hash;
	size_t i;

	for(hash = i = 0; i < len; ++i)
	{
		hash += value[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

bool
is_valid_email(char *email)
{
	regex_t regex;
	int32_t regc;

	regc = regcomp(&regex, "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+.[A-Za-z]{2,4}", REG_EXTENDED);

	if (regc != 0)
	{
		return false;
	}

	int32_t res;

	res = regexec(&regex, email, 0, NULL, 0);

	if (res == 0)
	{
		return true;
	}
	else if (res == REG_NOMATCH)
	{
		return false;
	}
	
	return false;

}

long long int
get_time_ms(void)
{
	struct timeb timer_msec;
	long long int timestamp_msec; 
	if (!ftime(&timer_msec)) 
	{
		timestamp_msec = ((long long int) timer_msec.time) * 1000ll + 
						(long long int) timer_msec.millitm;
	}
	else
	{
		timestamp_msec = -1;
	}
	
	return timestamp_msec;
}


