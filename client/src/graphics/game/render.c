#include <math.h>
#include <SDL2/SDL.h>
#include "../../engine.h"
#include "../../debug.h"
#include "render.h"
#include "util/log.h"
#include "game/tile_grid.h"

static struct renderer rndr;

struct renderer*
renderer(void)
{
	return &rndr;
}

void
render_init(void)
{

	rndr.bmp_font = bitmap_font_init("res/font/font.png", ' ', vec2_init(4, 6));

	rndr.bmp_font_title = bitmap_font_init(
			"res/font/font_title.png", 
			' ', 
			vec2_init(8, 11));

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		rndr.ren_heroes[i].hero = NULL;
	}

	rndr.grid_fog = tile_grid_init(vec2_init(120, 120), vec2_init(16, 16));

}

void
render_destroy(void)
{
	bitmap_font_destroy(rndr.bmp_font);
	SDL_DestroyTexture(rndr.spr_fog.tex.texture);
}

void
render_world_init(void)
{
 	SDL_Texture *tex = texture_get("res/art/world/map1.png");
	
	int32_t q_x = 0;
	int32_t q_y = 0;
	SDL_QueryTexture(
			tex,
			NULL,
			NULL,
			&q_x,
			&q_y);

	rndr.spr_map = sprite_init(
			tex, 
			NULL, 
			vec2_init(0, 0),
			vec2_init(0, 0), 
			vec2_init(q_x, q_y));	

	SDL_Texture *tex_fog = SDL_CreateTexture(
			graphics()->renderer, 
			SDL_PIXELFORMAT_RGBA8888, 
			SDL_TEXTUREACCESS_TARGET, 
			q_x, 
			q_y);

	rndr.spr_fog = sprite_init(
			tex_fog, 
			NULL, 
			vec2_init(0, 0),
			vec2_init(0, 0), 
			vec2_init(q_x, q_y));	

}

struct renderable_hero*
render_hero_init(struct hero *hero)
{
	struct renderable_hero ren_hero = renderable_hero_init(hero);

	struct renderable_hero *rh = NULL;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		rh = &rndr.ren_heroes[i];
		if (rh->hero == NULL)
		{
			*rh = ren_hero;
			return rh;
		}
	}
	
	return rh;
}

struct renderable_hero*
render_hero_get(uint32_t id_game)
{
	struct renderable_hero *ren_hero = NULL;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		ren_hero = &rndr.ren_heroes[i];
		if (ren_hero->hero == NULL) continue;

		if (ren_hero->hero->id_game == id_game) return ren_hero;
	}

	return ren_hero;
}

static void
render_sort_sprites(struct sprite **sprites, uint16_t sprite_count)
{
	if (sprite_count <= 1) return;

	bool swapped = false;

	for (int i = 0; i < sprite_count-1; i++)
	{
		swapped = false;

		for (int j = 0; j < sprite_count-1-i; j++)
		{

			struct sprite *spr1 = sprites[j];
			struct sprite *spr2 = sprites[j+1];
			if (spr1->z_layer > spr2->z_layer)
			{

				struct sprite *tmp_spr = spr1;
				sprites[j] = sprites[j+1];	
				sprites[j+1] = tmp_spr;
				swapped = true;
			}
			else if (spr1->z_layer == spr2->z_layer)
			{
				if (spr1->y_sort > spr2->y_sort)
				{
					struct sprite *tmp_spr = spr1;
					sprites[j] = sprites[j+1];	
					sprites[j+1] = tmp_spr;
					swapped = true;
				}
			}

		}

		if (!swapped) break;
	}
}

void
render_sprites(void)
{

	struct sprite *sprites[MAX_ENTITIES];
	uint16_t sprite_count = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct renderable_hero *ren_hero = &rndr.ren_heroes[i];
		if (ren_hero->hero == NULL) continue;
		
		if (ren_hero->render_data.spr.visible)
		{
			sprites[sprite_count] = &ren_hero->render_data.spr;
			sprite_count++;
		}
	}
	
	render_sort_sprites(sprites, sprite_count);
	
	for (int i = 0; i < sprite_count; i++)
	{
		sprite_render(sprites[i]);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct renderable_hero *ren_hero = &rndr.ren_heroes[i];
		if (ren_hero->hero == NULL) continue;
		
		renderable_hero_render(ren_hero);
	}

}

void
render_update(void)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct renderable_hero *ren_hero = &rndr.ren_heroes[i];
		if (ren_hero->hero == NULL) continue;
		
		renderable_hero_update(ren_hero);
	}
}

static void
render_fog_prepare(void)
{

	SDL_SetRenderTarget(graphics()->renderer, rndr.spr_fog.tex.texture);	
	SDL_SetRenderDrawBlendMode(graphics()->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(graphics()->renderer, 70, 70, 70, 255);
	SDL_RenderClear(graphics()->renderer);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct renderable_hero *ren_hero = &rndr.ren_heroes[i];
		if (ren_hero->hero == NULL) continue;
			
		vec2 si = ren_hero->render_data.spr_vision.tex.size;
		sprite_render_custom(&ren_hero->render_data.spr_vision, si);
		
	//	vec2 view = sprite_pos_view(&ren_hero->render_data.spr);
	//	SDL_Rect dst;
	//	dst.x = (int32_t)(ceil(view.x));
	//	dst.y = (int32_t)(ceil(view.y));
	//	dst.w = (int32_t)(ceil(si.x) / 4);
	//	dst.h = (int32_t)(ceil(si.y) / 4);
	//	
	//	SDL_Rect src = {0, 0, si.x, si.y};
	//	SDL_RenderCopy(
	//			graphics()->renderer, 
	//			ren_hero->render_data.spr_vision.tex.texture, 
	//			&src, 
	//			&dst);
	}


	SDL_SetTextureBlendMode(rndr.spr_fog.tex.texture, SDL_BLENDMODE_MOD);

	SDL_SetRenderTarget(graphics()->renderer, NULL);	
		

}

static void
render_fog(void)
{
	SDL_Rect dst;
//	dst.x = (int32_t)(round(0 * GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
//	dst.y = (int32_t)(round(0 * GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
	dst.x = 0;
	dst.y = 0;
	dst.w = (int32_t)(ceil(rndr.spr_map.tex.size.x * GAME_SCALE.x));
	dst.h = (int32_t)(ceil(rndr.spr_map.tex.size.y * GAME_SCALE.y));

	SDL_Rect src = {0, 0, rndr.spr_map.tex.size.x, rndr.spr_map.tex.size.y};
	SDL_RenderCopy(
			graphics()->renderer, 
			rndr.spr_fog.tex.texture, 
			&src, 
			&dst);

}

void
render_world(void)
{
//	render_fog_prepare();
	sprite_render(&rndr.spr_map);
	render_sprites();
//	sprite_render(&rndr.spr_fog);
//	render_fog();
}


//static void
//render_fog(void)
//{
//	for (int i = 0; i < MAX_PLAYERS; i++)
//	{
//		struct renderable_hero *ren_hero = &rndr.ren_heroes[i];
//		if (ren_hero->hero == NULL) continue;
//		
//		struct tile *ht = tile_get_f(&rndr.grid_fog, ren_hero->hero->pos);			
//
//		struct tile *left = tile_get(
//				&rndr.grid_fog, 
//				vec2_init(ht->pos.x - ren_hero->hero->sight, ht->pos.y)
//				);
//
//		struct tile *right = tile_get(
//				&rndr.grid_fog, 
//				vec2_init(ht->pos.x + ren_hero->hero->sight, ht->pos.y)
//				);
//
//		struct tile *top = tile_get(
//				&rndr.grid_fog, 
//				vec2_init(ht->pos.x, ht->pos.y - ren_hero->hero->sight)
//				);
//
//		struct tile *bottom = tile_get(
//				&rndr.grid_fog, 
//				vec2_init(ht->pos.x, ht->pos.y + ren_hero->hero->sight)
//				);
//		
//		log_trace("hero sight %d", ren_hero->hero->sight);
//		tile_print(ht, "hero");
//		tile_print(left, "left");
//		tile_print(right, "right");
//		tile_print(top, "top");
//		tile_print(bottom, "bottom");
//	
//		int32_t light = 150;
//		for (int y = top->pos.y; y <= bottom->pos.y; y++)
//		{
//			for (int x = left->pos.x; x <= right->pos.x; x++)
//			{
//				struct tile *t = tile_get(&rndr.grid_fog, vec2_init(x, y));
//
//				t->light = light;
//				light -= 5;
//			}
//		}
//	}
//
//	for (int32_t y = 0; y < rndr.grid_fog.size.y; y++)
//	{
//		for (int32_t x = 0; x < rndr.grid_fog.size.x; x++)
//		{
//			struct tile *t = tile_get(&rndr.grid_fog, vec2_init(x, y));			
//
//			SDL_SetRenderDrawColor(graphics()->renderer, 0, 0, 0, t->light);
//			SDL_Rect hitbox;
//		
//			hitbox.x = (int32_t)(ceil((t->pos.x * rndr.grid_fog.tile_size.x) *
//						GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
//			hitbox.y = (int32_t)(ceil((t->pos.y * rndr.grid_fog.tile_size.y) *
//						GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
//			hitbox.w = (int32_t)(rndr.grid_fog.tile_size.x * GAME_SCALE.x);
//			hitbox.h = (int32_t)(rndr.grid_fog.tile_size.y * GAME_SCALE.y);
//
//			//SDL_RenderDrawRect(graphics()->renderer, &hitbox);
//			SDL_RenderFillRect(graphics()->renderer, &hitbox);
//		}
//	}
//
//	for (int32_t y = 0; y < rndr.grid_fog.size.y; y++)
//	{
//		for (int32_t x = 0; x < rndr.grid_fog.size.x; x++)
//		{
//			struct tile *t = tile_get(&rndr.grid_fog, vec2_init(x, y));			
//			t->light = 150;
//		}
//	}
//	
//}


