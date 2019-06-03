#ifndef HERO_INFO__
#define HERO_INFO__

#define AVATAR_WIDTH 16
#define AVATAR_HEIGHT 16

#include <inttypes.h>
#include "../../util/game_values.h"

enum
hero_type
{
	HERO_TYPE_ARCHER,
};

enum
hero_action
{
	HERO_ACTION_IDLE,
	HERO_ACTION_WALK,
	HERO_ACTION_CAST_SPELL0,
	HERO_ACTION_CAST_SPELL1,
	HERO_ACTION_CAST_SPELL2,
	HERO_ACTION_CAST_SPELL3,
	HERO_ACTION_CAST_SPELL4,

	HERO_ACTION_COUNT
};

struct
heroes_in_range
{	
	struct hero *heroes_seen[MAX_PLAYERS];
	struct hero *heroes_seen_prev[MAX_PLAYERS];
};

struct
hero_archer
{
	uint8_t fire_arrow_count;
};

#endif



