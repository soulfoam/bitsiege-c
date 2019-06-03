#include <math.h>
#include "renderable_hero.h"
#include "../../debug.h"
#include "../graphics.h"
#include "camera.h"
#include "util/log.h"

struct renderable_hero
renderable_hero_init(struct hero *hero)
{
	struct renderable_hero ren_hero;

	ren_hero.hero = hero;
	ren_hero.render_data = *data_render_hero_get(hero->id_hero);
	ren_hero.render_data.spr.anim = 

		data_render_hero_get_anim(&ren_hero.render_data, hero->action, hero->dir);
	ren_hero.render_data.spr.visible = true;
		
	sprite_pos_set_all(&ren_hero.render_data.spr, hero->pos);
	sprite_pos_set_all(&ren_hero.render_data.spr_vision, ren_hero.hero->pos);
	
	animation_play(&ren_hero.render_data.spr.anim);

	return ren_hero;
}

void
renderable_hero_update(struct renderable_hero *ren_hero)
{
	struct hero *h = ren_hero->hero;
	sprite_pos_set(&ren_hero->render_data.spr, ren_hero->hero->pos);
	sprite_pos_set(&ren_hero->render_data.spr_vision, ren_hero->hero->pos);
//	log_trace(
//			"pos: %f %f %f %f vis", 
//			ren_hero->render_data.spr.pos_view.x,
//			ren_hero->render_data.spr.pos_view.y,
//			ren_hero->render_data.spr_vision.pos.x,
//			ren_hero->render_data.spr_vision.pos.y);

	ren_hero->render_data.spr.y_sort = 
		ren_hero->hero->pos.y + ren_hero->hero->data->hitbox_full.size.y;


	if (h->action != h->ss->action || h->dir != h->ss->dir)
	{
		struct animation *a = &ren_hero->render_data.spr.anim;	
		struct animation old = *a;
		*a = data_render_hero_get_anim(&ren_hero->render_data, h->action, h->dir);
		animation_play_from_animation(&ren_hero->render_data.spr.anim, old);
	}

}

void
renderable_hero_render(struct renderable_hero *ren_hero)
{

	if (!ren_hero->render_data.spr.visible) return;

	vec2 pos = sprite_pos_view(&ren_hero->render_data.spr);
	pos = vec2_add(pos, ren_hero->render_data.hp_bar_offset);
	vec2 hp_bar_size = vec2_init(18, 3);
	vec2 hp_size = vec2_init(
			hp_bar_size.x * (ren_hero->hero->hp / ren_hero->hero->hp_base), 
			hp_bar_size.y);

	SDL_Rect hp_bar;	
	hp_bar.x = (int32_t)(round(
				pos.x * GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
	hp_bar.y = (int32_t)(round(
				pos.y * GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
	hp_bar.w = (int32_t)(ceil(hp_bar_size.x * GAME_SCALE.x));
	hp_bar.h = (int32_t)(ceil(hp_bar_size.y * GAME_SCALE.y));

	SDL_Rect hp;	
	hp.x = (int32_t)(round(
				pos.x * GAME_SCALE.x - camera_pos().x * GAME_SCALE.x));
	hp.y = (int32_t)(round(
				pos.y * GAME_SCALE.y - camera_pos().y * GAME_SCALE.y));
	hp.w = (int32_t)(ceil(hp_size.x * GAME_SCALE.x));
	hp.h = (int32_t)(ceil(hp_size.y * GAME_SCALE.y));
  
	SDL_SetRenderDrawColor(graphics()->renderer, 0, 0, 0, 150);
	SDL_RenderFillRect(graphics()->renderer, &hp_bar);

	SDL_SetRenderDrawColor(graphics()->renderer, 255, 0, 0, 150);
	SDL_RenderFillRect(graphics()->renderer, &hp);
}

vec2
renderable_hero_camera_offset(struct renderable_hero *ren_hero)
{
	vec2 offset = vec2_init(
			((RES_WIDTH * -1) / 2) - (ren_hero->render_data.spr_offset.x / 2), 
			((RES_HEIGHT * -1) / 2) - (ren_hero->render_data.spr_offset.y / 2));

	return offset;
}



