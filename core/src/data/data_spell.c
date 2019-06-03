#include "data_spell.h"
#include "../game/spell/spell.h"
#include "../util/log.h"
#include "data_info.h"

static struct data_spell_cache spell_cache;

const struct data_spell_cache*
data_spell_cache(void)
{
	return &spell_cache;
}

const struct data_spell_set*
data_spell_set_get(enum hero_type hero_type)
{
	struct data_spell_set *set = NULL;

	for (int i = 0; i < DATA_SPELL_CACHE_SIZE; i++)
	{
		if (spell_cache.cache[i].hero_type == hero_type)
		{
			set = &spell_cache.cache[i];
			return set;
		}
	}

	log_warn("Spell set for hero %u unable to be found", hero_type);
	
	return NULL;
}

static void
data_spell_set_cooldowns(void)
{	
	for (int i = 0; i < spell_cache.spell_count; i++)
	{
		for (int j = 0; j < SPELL_SLOTS; j++)
		{
			float cast_time = 
				(spell_cache.cache[i].spells[j].cast_time) * TICKS_PER_SECOND;
			float cooldown = 
				(spell_cache.cache[i].spells[j].cooldown) * TICKS_PER_SECOND;

			spell_cache.cache[i].spells[j].cast_time = cast_time;
			spell_cache.cache[i].spells[j].cooldown = cooldown;
		}
	}
}

void
data_spell_init(void)
{
	uint16_t spell_count = 0;

	spell_cache.cache[spell_count++] = data_spell_init_archer();

	spell_cache.spell_count = spell_count;

	data_spell_set_cooldowns();
}

