#include <ctype.h>
#include <math.h>
#include "graphics.h"
#include "game/render.h"
#include "game/camera.h"
#include "bitmap_font.h"
#include "util/log.h"


struct bitmap_font*
bitmap_font_init(
		char *path, 
		char starting_char, 
		vec2 tile_size)
{
	struct bitmap_font *bmp_font = malloc(sizeof(struct bitmap_font));
	
	bmp_font->starting_char = starting_char;
	bmp_font->tile_size = tile_size;

	bmp_font->tex = IMG_LoadTexture(graphics()->renderer, path);
	
	if (bmp_font->tex == NULL)
	{
		log_fatal("Failed to load font.");
		return NULL;
	}	
	
	vec2i s = vec2i_init(0, 0);
	SDL_QueryTexture(
			bmp_font->tex,
			NULL,
			NULL,
			&s.x,
			&s.y);
	
	bmp_font->tex_size.x = s.x;
	bmp_font->tex_size.y = s.y;

	bmp_font->tile_count.x = s.x / tile_size.x;
	bmp_font->tile_count.y = s.y / tile_size.y;

	return bmp_font;
}

void
bitmap_font_destroy(struct bitmap_font *font)
{
	free(font);
}



struct bitmap_text*
bitmap_text_init(char *text, vec2 pos, struct bitmap_font *font, union color c)
{
	struct bitmap_text *t = malloc(sizeof(struct bitmap_text));
	strcpy(t->text, text);
	t->pos = pos;
	t->pos_prev = pos;
	t->font = font;
	t->color = c;

	return t;
}

void
bitmap_text_pos_set(struct bitmap_text *text, vec2 pos)
{
	text->pos_prev = text->pos;	
	text->pos = pos;	
}

static void
bitmap_text_render_base(struct bitmap_text *t, bool cam, uint8_t alpha)
{
	
	struct bitmap_font *font = t->font;
	char *text = t->text;
	vec2 pos = t->pos;
	union color color = t->color;

	if (t->pos_prev.x != t->pos.x || t->pos_prev.y != t->pos.y)
	{
		pos = vec2_lerp(t->pos_prev, t->pos, R_PERC);	
	}	

	int32_t text_len = strlen(text);
	int32_t tile_count_total = font->tile_count.x * font->tile_count.y;
	
	for (int32_t i = 0; i < text_len; i++)
	{
		int32_t index = (int32_t)text[i] - (int32_t)font->starting_char;	

		if (index < tile_count_total)
		{
			vec2 tile_pos = vec2_init(
					fmod(index, font->tile_count.x), 
					index / font->tile_count.x);

			SDL_Rect src = {
				tile_pos.x * font->tile_size.x, 
				tile_pos.y * font->tile_size.y, 
				font->tile_size.x, 
				font->tile_size.y}; 
			

			int32_t f_x = pos.x + (i * font->tile_size.x);
			int32_t f_y = pos.y;

			SDL_Rect dst = {f_x, 
							f_y, 
							font->tile_size.x, 
							font->tile_size.y};

			
			if ((i >= 0) || (i <= text_len-1))
			{	
				if (cam)
				{	
					f_x = round(f_x * GAME_SCALE.x - camera_pos().x * GAME_SCALE.x);
					f_y = round(f_y * GAME_SCALE.y - camera_pos().y * GAME_SCALE.y);
				}
				else
				{
					f_x = round(f_x * GAME_SCALE.x);
					f_y = round(f_y * GAME_SCALE.y);
				}
				
				dst.x = (int32_t)(f_x);
				dst.y = (int32_t)(f_y);
				dst.w = (int32_t)(ceil(font->tile_size.x * GAME_SCALE.x));
				dst.h = (int32_t)(ceil(font->tile_size.y * GAME_SCALE.y));

				SDL_SetTextureColorMod(
						font->tex, 
						color.val.r, 
						color.val.g, 
						color.val.b);
				
				SDL_SetTextureAlphaMod(font->tex, alpha);

				SDL_RenderCopyEx(
						graphics()->renderer, 
						font->tex, 
						&src, 
						&dst, 
						0, 
						NULL, 
						SDL_FLIP_NONE);
			}
		}
	}
	

}

void
bitmap_text_render_cam(struct bitmap_text *text)
{
	bitmap_text_render_base(text, true, 255);
}

void
bitmap_text_render_cam_alpha(struct bitmap_text *text, uint8_t alpha)
{
	bitmap_text_render_base(text, true, alpha);
}

void
bitmap_text_render(struct bitmap_text *text)
{
	bitmap_text_render_base(text, false, 255);
}

void
bitmap_text_render_alpha(struct bitmap_text *text, uint8_t alpha)
{
	bitmap_text_render_base(text, false, alpha);
}

uint32_t
bitmap_text_width(struct bitmap_font *font, const char *text)
{
	uint32_t len = strlen(text);
	return (font->tile_size.x) * len;
}

