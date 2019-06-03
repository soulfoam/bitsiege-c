#include "data_hero.h"
#include "data_spell.h"
#include "../game/spell/spell.h"
#include "../game/hero/hero_info.h"
#include "../util/log.h"
#include "../game/direction.h"
#include "../data/data_info.h"

static struct data_hero_cache hero_cache;

const struct data_hero_cache*
data_hero_cache(void)
{
	return &hero_cache;
}

const struct data_hero*
data_hero_get(enum hero_type type)
{
	for (int i = 0; i < DATA_HERO_CACHE_SIZE; i++)
	{
		if (hero_cache.cache[i].type == type)
		{
			return &hero_cache.cache[i];
		}
	}
	
	return NULL;
}

void
data_hero_init(void)
{
	uint16_t hero_count = 0;

	hero_cache.cache[hero_count++] = data_hero_init_archer();

	hero_cache.hero_count = hero_count;
}




