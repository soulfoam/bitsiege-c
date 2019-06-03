#ifndef data__
#define data__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include <dirent.h>
#include "../util/game_values.h"
#include "../game/hitbox.h"
#include "../game/hero/hero_info.h"
#include "../game/spell/spell_info.h"

struct
data_hero
{
	enum hero_type type;
	char name[MAX_NAME_LEN+1];
	
	struct hitbox hitbox_full;
	struct hitbox hitbox_body;
	struct hitbox hitbox_feet;

	uint16_t hp;
	float move_speed;
	uint16_t sight;
	uint16_t damage_physical;
	uint16_t damage_magic;
	uint8_t resist_physical;
	uint8_t resist_magic;

	const struct data_spell *spells[SPELL_SLOTS];
	
};

struct
data_hero_cache
{
	struct data_hero cache[DATA_HERO_CACHE_SIZE];
	uint16_t hero_count;
};

void
data_hero_init(void);

const struct data_hero*
data_hero_get(enum hero_type type);

const struct data_hero_cache*
data_hero_cache(void);

#endif



