#ifndef DATA_RENDER_SPELL__
#define DATA_RENDER_SPELL__

#include "game/spell/spell_info.h"
#include "util/game_values.h"
#include "inttypes.h"
#include "../game/animation.h"
#include "../game/sprite.h"
#include "util/vec2.h"

struct 
data_render_spell
{
	enum spell_type type;
	struct animation anims[2];
	
	uint16_t spr_frame_size;
	vec2 spr_offset;
	struct sprite spr;
};

struct
data_render_spell_cache
{
	struct data_render_spell cache[DATA_SPELL_CACHE_SIZE];
	uint16_t render_spell_count;
};


#endif



