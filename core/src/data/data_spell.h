#ifndef DATA_SPELL__
#define DATA_SPELL__

#include "../util/vec2.h"
#include "../game/spell/spell_info.h"
#include "../game/hero/hero_info.h"
#include "../game/hitbox.h"
#include "../util/game_values.h"

#define SPELL_SLOT0 0
#define SPELL_SLOT1 1
#define SPELL_SLOT2 2
#define SPELL_SLOT3 3
#define SPELL_SLOT4 4

struct
data_spell
{
	enum spell_type type;
	float cooldown;
	float cast_time;
	bool channeled;
	struct hitbox hitbox;
	bool remove_on_death;
};

struct
data_spell_set
{
	enum hero_type hero_type;
	struct data_spell spells[SPELL_SLOTS];
};

struct
data_spell_cache
{
	struct data_spell_set cache[DATA_SPELL_CACHE_SIZE];
	uint16_t spell_count;
};

const struct data_spell_cache*
data_spell_cache(void);

const struct data_spell_set*
data_spell_set_get(enum hero_type hero_type);

void
data_spell_init(void);


#endif


