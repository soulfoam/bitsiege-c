#ifndef _INPUT__
#define _INPUT__

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "game/hero/hero_info.h"
#include "game/direction.h"
#include "util/vec2.h"

enum
input_game_flag
{
	FLAG_GAME_HITBOX_SHOW = 1 << 0,
	FLAG_GAME_CAMERA_TRACE = 1 << 1,

	
	FLAG_GAME_COUNT
};

struct
input
{
	uint16_t flags_input_prev;
	uint16_t flags_input;
	uint8_t flags_game;

	bool mouse_click_hold;

	enum direction mouse_dir_prev;
	enum direction mouse_dir;

};

struct input*
input(void);

void
input_init(void);

void
input_process(SDL_Event *e);

void
input_update(void);

vec2
input_mouse_pos_screen(void);

vec2
input_mouse_pos_world(void);

void
input_mouse_dir_set(vec2 pos);

#endif


