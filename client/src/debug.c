#include <math.h>
#include "debug.h"
#include "graphics/game/camera.h"
#include "engine.h"
#include "util/log.h"

static struct debug_values values;

const struct debug_values*
debug_values(void)
{
	return &values;
}

void
debug_camera_trace(void)
{
	values.camera_trace = !values.camera_trace;
}

void
debug_hitbox(void)
{
	values.hitbox_show = !values.hitbox_show;
}

void
debug_update(void)
{
	if (values.camera_trace)
	{

		log_trace(
		"Pos: %f %f\nPos Prev: %f %f\nView(+Offset): %f %f(%f %f)\n   \
		View: %f %f\nSpeed: %f\nLocked: %d", 
		camera()->pos.x,
		camera()->pos.y,
		camera()->pos_prev.x,
		camera()->pos_prev.y,
		camera_pos().x,
		camera_pos().y,
		camera()->offset.x,
		camera()->offset.y,
		camera()->pos_view.x,
		camera()->pos_view.y,
		camera()->speed,
		camera()->locked);
	}
}

static void
debug_render_hitboxes(void)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct renderable_hero *ren_hero = &renderer()->ren_heroes[i];
		if (ren_hero->hero != NULL)
		{
			if (debug_values()->hitbox_show)
			{

				SDL_Rect hitbox;
				vec2 full_off = ren_hero->hero->hitboxes[HERO_HITBOX_FULL].pos_offset;
				vec2 body_off = ren_hero->hero->hitboxes[HERO_HITBOX_BODY].pos_offset;
				vec2 feet_off = ren_hero->hero->hitboxes[HERO_HITBOX_FEET].pos_offset;
				struct sprite *spr = &ren_hero->render_data.spr;	

				SDL_SetRenderDrawColor(graphics()->renderer, 255, 0, 0, 255);
				hitbox.x = (int32_t)
					(round((spr->pos_view.x + full_off.x) * 
						   GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
				hitbox.y = (int32_t)
					(round((spr->pos_view.y + full_off.y) * 
						   GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
				hitbox.w = (int32_t)
					(ceil((ren_hero->hero->hitboxes[HERO_HITBOX_FULL].size.x) * GAME_SCALE.x));
				hitbox.h = (int32_t)
					(ceil((ren_hero->hero->hitboxes[HERO_HITBOX_FULL].size.y) * GAME_SCALE.y));
				SDL_RenderDrawRect(graphics()->renderer, &hitbox);

				SDL_SetRenderDrawColor(graphics()->renderer, 255, 255, 255, 255);
				hitbox.x = (int32_t)
					(round((spr->pos_view.x + body_off.x) * 
						   GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
				hitbox.y = (int32_t)
					(round((spr->pos_view.y + body_off.y) * 
						   GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
				hitbox.w = (int32_t)
					(ceil((ren_hero->hero->hitboxes[HERO_HITBOX_BODY].size.x) * GAME_SCALE.x));
				hitbox.h = (int32_t)
					(ceil((ren_hero->hero->hitboxes[HERO_HITBOX_BODY].size.y) * GAME_SCALE.y));
				SDL_RenderDrawRect(graphics()->renderer, &hitbox);

				SDL_SetRenderDrawColor(graphics()->renderer, 0, 255, 255, 255);
				hitbox.x = (int32_t)
					(round((spr->pos_view.x + feet_off.x) * 
						   GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
				hitbox.y = (int32_t)
					(round((spr->pos_view.y + feet_off.y) * 
						   GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
				hitbox.w = (int32_t)
					(ceil((ren_hero->hero->hitboxes[HERO_HITBOX_FEET].size.x) * GAME_SCALE.x));
				hitbox.h = (int32_t)
					(ceil((ren_hero->hero->hitboxes[HERO_HITBOX_FEET].size.y) * GAME_SCALE.y));
				SDL_RenderDrawRect(graphics()->renderer, &hitbox);
			}
		}
	}
}

static void
debug_render_tile_grid(void)
{
	if (debug_values()->hitbox_show)
	{
		struct tile_grid *grid = &eng()->game->world.grid;

		for (int32_t y = 0; y < grid->size.y; y++)
		{
			for (int32_t x = 0; x < grid->size.x; x++)
			{
				struct tile *t = tile_get(grid, vec2_init(x, y));			
				SDL_SetRenderDrawColor(graphics()->renderer, 0, 255, 0, 255);
				SDL_Rect hitbox;
			
				hitbox.x = (int32_t)(round((t->pos.x * grid->tile_size.x) *
							GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
				hitbox.y = (int32_t)(round((t->pos.y * grid->tile_size.y) *
							GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
				hitbox.w = (int32_t)(grid->tile_size.x * GAME_SCALE.x);
				hitbox.h = (int32_t)(grid->tile_size.y * GAME_SCALE.y);

				SDL_RenderDrawRect(graphics()->renderer, &hitbox);
			}
		}
	}
}

void
debug_render(void)
{
	debug_render_tile_grid();
	debug_render_hitboxes();
}


