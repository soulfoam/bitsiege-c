#ifndef TEXTBOX__
#define TEXTBOX__


#include <SDL2/SDL.h>

#include "graphics.h"

struct
textbox
{
	char *text;

	SDL_Color color_text;
	SDL_Color color_background;

	vec2 pos;
	vec2 size;	
};

struct textbox*
textbox_init(const struct graphics *gfx, vec2 pos, vec2 size);

void
textbox_destroy(struct textbox *textbox);

void
textbox_render(struct textbox *textbox, const struct graphics *gfx);

void
textbox_set_text_size(struct textbox *textbox, uint16_t size);

#endif
