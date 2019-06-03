#ifndef GAME_UTIL__
#define GAME_UTIL__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vec2.h"
#include "../game/direction.h"
#include "../game/hero/hero_info.h"

union
color
{
	uint32_t c;

	struct 
	val
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	} val;
};

enum
input_bit_flag
{
	FLAG_INPUT_UP = 1 << 0,
	FLAG_INPUT_LEFT = 1 << 1,
	FLAG_INPUT_DOWN = 1 << 2,
	FLAG_INPUT_RIGHT = 1 << 3,
	FLAG_INPUT_CAST0 = 1 << 4,
	FLAG_INPUT_CAST1 = 1 << 5,
	FLAG_INPUT_CAST2 = 1 << 6,
	FLAG_INPUT_CAST3 = 1 << 7,
	FLAG_INPUT_CAST4 = 1 << 8,
	FLAG_INPUT_CAST_CANCEL= 1 << 9,

	FLAG_INPUT_COUNT
};

union color
color_init(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

union color
color_init_c(uint32_t c);

enum direction
move_dir_from_input(uint16_t input);

vec2
vec2_from_dir(enum direction dir);

double
face_dir(vec2 dst, vec2 pos);

enum direction
dir_from_mouse(vec2 mouse_pos, vec2 pos);

enum hero_action
hero_action_from_input(uint16_t input);

enum hero_action
hero_action_cast_from_slot(uint8_t slot);

#endif



