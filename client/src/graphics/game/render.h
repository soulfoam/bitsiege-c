#ifndef RENDER__
#define RENDER__

#include "game/world.h"
#include "game/tile_grid.h"
#include "../graphics.h"
#include "../game/renderable_hero.h"
#include "camera.h"
#include "../bitmap_font.h"
#include "util/s_list.h"

struct
renderer
{	
	struct sprite spr_map;
	struct sprite spr_fog;

	struct renderable_hero ren_heroes[MAX_PLAYERS];	

	struct bitmap_font *bmp_font;
	struct bitmap_font *bmp_font_title;

	struct tile_grid grid_fog;
};

struct renderer*
renderer(void);

void
render_init(void);

void
render_destroy(void);

void
render_world_init(void);

struct renderable_hero*
render_hero_init(struct hero *hero);

struct renderable_hero*
render_hero_get(uint32_t id_game);

void
render_update(void);

void
render_world(void);


#endif


