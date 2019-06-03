#include <stdlib.h>
#include "id_pool.h"
#include "../util/log.h"
#include "limits.h"

uint16_t*
id_pool_init(uint16_t size)
{
	uint16_t *pool = malloc(size * sizeof(uint16_t)); 

	for (uint16_t i = 0; i < size; i++)
	{
		pool[i] = i;
	}

	return pool;
}

uint16_t
id_pool_get(uint16_t *pool, uint16_t size)
{
	for (uint16_t i = 0; i < size; i++)
	{
		if (pool[i] != 0)
		{
			uint16_t id = pool[i];
			pool[i] = 0;
			return id;
		}
	}

	log_error("No more ids in the pool, SHIT.");

	return -1;
}

void
id_pool_put(uint16_t *pool, uint16_t id, uint16_t size)
{
	for (int i = 0; i < size; i++)
	{
		if (pool[i] == 0)
		{
			pool[i] = id;
			return;
		}
	}
}

void
id_pool_destroy(uint16_t *pool)
{
	free(pool);	
}

