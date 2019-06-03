#ifndef DATA_RENDER_HERO__
#define DATA_RENDER_HERO__


#include <SDL2/SDL.h>
#include "data/data_hero.h"
#include "../game/sprite.h"
#include "game/hero/hero_info.h"

#define RENDER_HERO_ANIM_SIZE 56

struct
data_render_hero_anim
{
	enum hero_action action;
	enum direction dir;
	struct animation anim;
};

struct
data_render_hero
{
	enum hero_type type;
	
	uint16_t animation_count;
	struct data_render_hero_anim anims[RENDER_HERO_ANIM_SIZE];
	
	vec2 hp_bar_offset;
	uint16_t spr_frame_size;
	vec2 spr_offset;
	struct sprite spr;	

	struct sprite spr_avatar;	
	struct sprite spr_vision;

};

struct
data_render_hero_cache
{
	struct data_render_hero cache[DATA_HERO_CACHE_SIZE];
	uint16_t render_hero_count;
};

const struct data_render_hero*
data_render_hero_get(enum hero_type hero_type);

void
data_render_hero_init(void);

struct animation
data_render_hero_get_anim(
		const struct data_render_hero *data,
		enum hero_action action, 
		enum direction dir);

#endif


