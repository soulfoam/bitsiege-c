#ifndef BITMAP_FONT__
#define BITMAP_FONT__

#include <stdbool.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "util/vec2.h"
#include "util/game_util.h"

struct
bitmap_font
{
	SDL_Texture *tex;
	vec2 tex_size;

	uint16_t horizontal_total_count;

	vec2 tile_size;
	vec2 tile_count;

	char starting_char;
};

struct bitmap_font*
bitmap_font_init(
		char *path, 
		char starting_char, 
		vec2 tile_size);

void
bitmap_font_destroy(struct bitmap_font *font);


struct
bitmap_text
{
	struct bitmap_font *font;
	union color color;
	vec2 pos;
	vec2 pos_prev;
	char text[MAX_TEXT_LEN];

};

struct bitmap_text*
bitmap_text_init(char *text, vec2 pos, struct bitmap_font *font, union color c);

void
bitmap_text_pos_set(struct bitmap_text *text, vec2 pos);

void
bitmap_text_render_cam(struct bitmap_text *text);

void
bitmap_text_render_cam_alpha(struct bitmap_text *text, uint8_t alpha);

void
bitmap_text_render(struct bitmap_text *text);

void
bitmap_text_render_alpha(struct bitmap_text *text, uint8_t alpha);

uint32_t
bitmap_text_width(struct bitmap_font *font, const char *text);

#endif



