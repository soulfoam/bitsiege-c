#include <math.h>
#include "sprite.h"
#include "texture.h"
#include "util/log.h"
#include "game/direction.h"
#include "../../delta_time.h"
#include "../graphics.h"
#include "../../debug.h"
#include "camera.h"

struct sprite
sprite_init(
		SDL_Texture *texture, 
		const struct animation *anim, 
		vec2 pos,
		vec2 pos_offset,
		vec2 size)
{
	struct sprite sprite;

	sprite.dst.x = pos.x;
	sprite.dst.y = pos.y;

	sprite_pos_set_all(&sprite, pos);
	sprite.pos_offset = pos_offset;

	sprite.dst.w = size.x;
	sprite.dst.h = size.y;
	sprite.size = vec2_init(size.x, size.y);
	sprite.tex.texture = texture;	

	vec2i s = vec2i_init(0, 0);
	SDL_QueryTexture(
			sprite.tex.texture,
			NULL,
			NULL,
			&s.x,
			&s.y);

	sprite.tex.size = vec2_init(s.x, s.y);

	sprite.animated = false;

	if (anim != NULL)
	{
		sprite.anim = *anim;
		sprite.animated = true;
	}
	
	sprite.y_sort = 0;
	sprite.z_layer = 0;
	sprite.visible = true;

	return sprite;

}

vec2
sprite_pos_view(struct sprite *spr)
{
	vec2 offset = vec2_init(spr->pos_offset.x, spr->pos_offset.y);
	return vec2_add(spr->pos_view, offset);	
}

void
sprite_pos_set(struct sprite *spr, vec2 pos)
{
	spr->pos_prev = spr->pos;
	spr->pos = pos;	
}

void
sprite_pos_set_all(struct sprite *spr, vec2 pos)
{
	spr->pos = pos;
	spr->pos_prev = pos;
	spr->pos_view = pos;
}

void
sprite_render_custom(struct sprite *spr, vec2 size)
{
	if (!spr->visible) return;
	
	spr->pos_view = vec2_lerp(spr->pos_prev, spr->pos, R_PERC);	

	vec2 view = sprite_pos_view(spr);
	
	SDL_Rect dst;
	dst.x = (int32_t)(ceil(view.x));
	dst.y = (int32_t)(ceil(view.y));
	dst.w = (int32_t)(ceil(size.x));
	dst.h = (int32_t)(ceil(size.y));

	SDL_Rect src = {0, 0, spr->tex.size.x, spr->tex.size.y};
	SDL_RenderCopy(
			graphics()->renderer, 
			spr->tex.texture, 
			&src, 
			&dst);
}

void
sprite_render(struct sprite *spr)
{
	if (!spr->visible) return;
	
	spr->pos_view = vec2_lerp(spr->pos_prev, spr->pos, R_PERC);	

	vec2 view = sprite_pos_view(spr);

	spr->dst.x = (int32_t)(round((view.x * GAME_SCALE.x) - (camera_pos().x * GAME_SCALE.x)));
	spr->dst.y = (int32_t)(round((view.y * GAME_SCALE.y) - (camera_pos().y * GAME_SCALE.y)));
	spr->dst.w = (int32_t)(ceil(spr->size.x * GAME_SCALE.x));
	spr->dst.h = (int32_t)(ceil(spr->size.y * GAME_SCALE.y));

	if (!spr->animated)
	{
		SDL_Rect src = {0, 0, spr->tex.size.x, spr->tex.size.y};
		SDL_RenderCopy(
				graphics()->renderer, 
				spr->tex.texture, 
				&src, 
				&spr->dst);
	}
	else
	{

		animation_render_update(&spr->anim);

		SDL_RenderCopy(
			graphics()->renderer, 
			spr->tex.texture, 
			&spr->anim.src, 
			&spr->dst);
	}
	
	if (debug_values()->hitbox_show)
	{
		SDL_SetRenderDrawColor(graphics()->renderer, 255, 255, 0, 255);
		SDL_Rect hitbox;
		hitbox.x = (int32_t)(round((view.x * GAME_SCALE.x) - (camera_pos().x * GAME_SCALE.x)));
		hitbox.y = (int32_t)(round((view.y * GAME_SCALE.y) - (camera_pos().y * GAME_SCALE.y)));
		hitbox.w = (int32_t)(ceil((spr->size.x) * GAME_SCALE.x));
		hitbox.h = (int32_t)(ceil((spr->size.y) * GAME_SCALE.y));

		SDL_RenderDrawRect(graphics()->renderer, &hitbox);
	}
}

		

