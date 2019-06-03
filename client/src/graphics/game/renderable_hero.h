#ifndef RENDERABLE_HERO__
#define RENDERABLE_HERO__

#include "game/hero/hero.h"
#include "game/hero/hero_info.h"
#include "../data/data_render_hero.h"

struct
renderable_hero
{
	struct hero *hero;
	struct data_render_hero render_data;
};

struct renderable_hero
renderable_hero_init(struct hero *hero);

void
renderable_hero_update(struct renderable_hero *ren_hero);

void
renderable_hero_render(struct renderable_hero *ren_hero);

vec2
renderable_hero_camera_offset(struct renderable_hero *ren_hero);

#endif


