#include <limits.h>
#include "data_render_hero.h"
#include "util/game_values.h"
#include "util/log.h"
#include "../../paths.h"

//Data anim order
//idle
//walk
//auto attack

static struct data_render_hero_cache render_hero_cache;

const struct data_render_hero*
data_render_hero_get(enum hero_type hero_type)
{
	for (int i = 0; i < render_hero_cache.render_hero_count; i++)
	{
		if (render_hero_cache.cache[i].type == hero_type)
		{
			return &render_hero_cache.cache[i];
		}
	}
	
	log_fatal("Couldn't get hero render data for type: %d", hero_type);

	return NULL;
}

static struct data_render_hero_anim
data_render_hero_anim_init(enum hero_action action, enum direction dir, struct animation anim)
{
	struct data_render_hero_anim a;
	a.action = action;
	a.dir = dir;
	a.anim = anim;

	return a;
}

static void
data_render_hero_spr_vision_init(struct sprite *spr)
{
 	SDL_Texture *tex_vision = texture_get("res/art/world/vision.png");

	int32_t q_x = 0;
	int32_t q_y = 0;
	SDL_QueryTexture(
			tex_vision,
			NULL,
			NULL,
			&q_x,
			&q_y);

	vec2 off = vec2_init(((q_x/4) / 2) * -1, ((q_y/4) / 2) * -1);
	
	*spr = sprite_init(
			tex_vision, 
			NULL, 
			vec2_init(-1000, -1000),
			off,
			vec2_init(q_x, q_y));
}

static struct data_render_hero
data_render_hero_init_archer(void)
{
	struct data_render_hero data;
	data.type = HERO_TYPE_ARCHER;
	
	uint16_t anim_index = 0;
	uint16_t frame_index = 0;
	
	for (int i = 0; i < DIR_COUNT; i++)
	{
		uint16_t fc = 1;

		struct animation anim = animation_init(frame_index, fc, 100, true, 32);
		struct data_render_hero_anim a = 
			data_render_hero_anim_init(HERO_ACTION_IDLE, i, anim);
	
		frame_index += fc;
		anim_index++;

		data.anims[anim_index] = a;		
	}
	
	for (int i = 0; i < DIR_COUNT; i++)
	{
		uint16_t fc = 8;

		struct animation anim = animation_init(frame_index, fc, 100, true, 32);
		struct data_render_hero_anim a = 
			data_render_hero_anim_init(HERO_ACTION_WALK, i, anim);
	
		frame_index += fc;
		anim_index++;

		data.anims[anim_index] = a;		
	}

	for (int i = 0; i < DIR_COUNT; i++)
	{
		uint16_t fc = 7;

		struct animation anim = animation_init(frame_index, fc, 100, true, 32);
		struct data_render_hero_anim a = 
			data_render_hero_anim_init(HERO_ACTION_CAST_SPELL0, i, anim);
	
		frame_index += fc;
		anim_index++;
		
		data.anims[anim_index] = a;		
	}

	data.hp_bar_offset = vec2_init(6, -3);
	data.spr_offset = vec2_init(-11, -5);
	data.spr_frame_size = 32;

	data.spr = sprite_init(
			texture_get(PATH_ART_HERO_ARCHER), 
			&data.anims[0].anim, 
			vec2_init(-1000, -1000),
			data.spr_offset,
			vec2_init(32, 32));

	data.spr_avatar = sprite_init(
			texture_get(PATH_ART_AVATAR_ARCHER),
			NULL,
			vec2_init(-1000, -1000),
			vec2_init(0, 0),
			vec2_init(AVATAR_WIDTH, AVATAR_HEIGHT)
			);

	data_render_hero_spr_vision_init(&data.spr_vision);

	return data;
}

void
data_render_hero_init(void)
{
	const struct data_hero_cache *hero_cache = data_hero_cache();
	render_hero_cache.render_hero_count = hero_cache->hero_count;

	uint16_t render_count = 0;	

	render_hero_cache.cache[render_count++] = data_render_hero_init_archer();

	log_info("Hero render data initialized.");
}

struct animation
data_render_hero_get_anim(
		const struct data_render_hero *data,
		enum hero_action action, 
		enum direction dir)
{
	struct animation anim;
	anim.frame_width = UINT_MAX;

	for (int i = 0; i < data->animation_count; i++)
	{
		const struct data_render_hero_anim *a = &data->anims[i];
		if (a->action == action && a->dir == dir) return a->anim;
	}
	
	log_warn(
			"Unable to get animation for hero type: %d. Action %d. Dir %d", 
			data->type, 
			action, 
			dir);

	return anim;
}


