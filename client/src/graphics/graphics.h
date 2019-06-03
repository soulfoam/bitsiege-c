#ifndef GRAPHICS__
#define GRAPHICS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string.h>
#include <stdbool.h>
#include "../delta_time.h"
#include "game/texture.h"
#include "game/animation.h"
#include "game/sprite.h"
#include "util/vec2.h"
#include "util/game_values.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

struct
graphics
{
	SDL_Window *window;
	SDL_Renderer *renderer;
};

struct graphics*
graphics(void);

void
graphics_init(void);

void
graphics_destroy(void);

void
graphics_calculate_scale(void);

#ifdef __EMSCRIPTEN__
void
graphics_calculate_html(void);

EM_BOOL
graphics_resize_html(int eventType, 
		const EmscriptenUiEvent *e, 
		void *userData);
#endif


extern vec2 GAME_SCALE;
extern vec2 WINDOW_SIZE;

#endif

