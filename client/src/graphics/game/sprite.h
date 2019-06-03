#ifndef SPRITE__
#define SPRITE__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "animation.h"
#include "util/vec2.h"

struct
sprite_tex
{
	SDL_Texture *texture;
	vec2 size;
};

struct
sprite
{
	SDL_Rect dst;

	vec2 pos_offset;
	vec2 pos_view;
	vec2 pos;
	vec2 pos_prev;

	vec2 size;

	bool animated;
	struct animation anim;

	struct sprite_tex tex;

	uint16_t y_sort;
	uint8_t z_layer;

	bool visible;
};

struct sprite
sprite_init(
		SDL_Texture *texture, 
		const struct animation *anim, 
		vec2 pos,
		vec2 pos_offset,
		vec2 size);


vec2
sprite_pos_view(struct sprite *spr);

void
sprite_pos_set(struct sprite *spr, vec2 pos);

void
sprite_pos_set_all(struct sprite *spr, vec2 pos);

void
sprite_render_custom(struct sprite *spr, vec2 size);

void
sprite_render(struct sprite *spr);

#endif


