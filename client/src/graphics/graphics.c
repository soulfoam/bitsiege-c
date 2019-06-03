#include <math.h>
#include "../engine.h"
#include "graphics.h"
#include "util/log.h"
#include "../debug.h"

vec2 GAME_SCALE;
vec2 WINDOW_SIZE;

static struct graphics gfx;

struct graphics*
graphics(void)
{
	return &gfx;
}

void
graphics_init(void)
{
	gfx.renderer = NULL;
	gfx.window = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{	
		log_error("SDL Video failed to create %s.", SDL_GetError());
	}

	gfx.window = SDL_CreateWindow(
			"soulengine",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			RES_WIDTH * 3,
			RES_HEIGHT * 3,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
			| SDL_WINDOW_INPUT_GRABBED);

	gfx.renderer = SDL_CreateRenderer(
			gfx.window,
			-1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
			| SDL_RENDERER_TARGETTEXTURE);


	if (gfx.window == NULL)
	{
		log_fatal("SDL window failed to create: %s.", SDL_GetError());
	}
	if (gfx.renderer == NULL)
	{
		log_fatal("SDL renderer failed to create: %s.", SDL_GetError());
	}	

	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == 0)
	{
		log_error("Couldn't set SDL Hint for nearest neighbor.");	
	}

	SDL_SetRenderDrawColor(gfx.renderer, 0xAA, 0xFF, 0xFF, 0xFF);

	#ifndef __EMSCRIPTEN__
	int32_t img_flags = IMG_INIT_PNG;
	if (!(IMG_Init(img_flags) & img_flags))
	{
		log_fatal("SDL image init failed: %s", IMG_GetError());
	}
	#endif

	SDL_SetRenderDrawBlendMode(gfx.renderer, SDL_BLENDMODE_BLEND);

	texture_init(gfx.renderer);
	graphics_calculate_scale();

	log_info("Graphics initialized.");
}

void
graphics_destroy(void)
{	
	texture_destroy();
	SDL_DestroyRenderer(gfx.renderer);
	SDL_DestroyWindow(gfx.window);

}

void
graphics_calculate_scale(void)
{
	int32_t w = 0;
	int32_t h = 0;

	SDL_GL_GetDrawableSize(gfx.window, &w, &h);

	WINDOW_SIZE.x = w;
	WINDOW_SIZE.y = h;

	GAME_SCALE.x = round(w / RES_WIDTH);
	GAME_SCALE.y = round(h / RES_HEIGHT);
	
	int32_t scale = fmax(GAME_SCALE.x, GAME_SCALE.y);
	if (scale <= 0) scale = 1;
	GAME_SCALE.x = scale; 
	GAME_SCALE.y = scale; 
}

#ifdef __EMSCRIPTEN__
void
graphics_calculate_html(void)
{
	double w = 0;
	double h = 0;
	emscripten_get_element_css_size("canvas", &w, &h);
	emscripten_set_canvas_element_size("canvas", w, h);	
	double dpr = emscripten_get_device_pixel_ratio();
	w *= dpr;
	h *= dpr;
	SDL_SetWindowSize(gfx.window, (int)w, (int)h);
	graphics_calculate_scale(gfx);
}

EM_BOOL
graphics_resize_html(int eventType, const EmscriptenUiEvent *e, void *userData)
{
	graphics_calculate_html(userData);
}
#endif

