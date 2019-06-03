#include <math.h>
#include "game_util.h"
#include "../util/log.h"
#include "../game/spell/spell.h"

union color
color_init(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	union color c;
	c.val.r = r;
	c.val.g = g;
	c.val.b = b;
	c.val.a = a;
	
	return c;
}

union color
color_init_c(uint32_t c)
{
	union color col;
	col.c = c;

	return col;
}

enum direction
move_dir_from_input(uint16_t input)
{	
	bool up = input & FLAG_INPUT_UP;
	bool left = input & FLAG_INPUT_LEFT;
	bool down = input & FLAG_INPUT_DOWN;
	bool right = input & FLAG_INPUT_RIGHT;
	

	if (up && left) return DIR_NW;	
	if (up && right) return DIR_NE;	
	if (down && left) return DIR_SW;
	if (down && right) return DIR_SE;
	if (up) return DIR_N;	
	if (left) return DIR_W;
	if (down) return DIR_S;
	if (right) return DIR_E;
	
	return DIR_NONE;
}

vec2
vec2_from_dir(enum direction dir)
{	
	vec2 pos;
	pos.x = 0;
	pos.y = 0;
	
	switch (dir)
	{
		case DIR_N:
			pos.x = 0;
			pos.y = -1;
		break;

		case DIR_W:
			pos.x = -1;
			pos.y = 0;
		break;	

		case DIR_S:
			pos.x = 0;
			pos.y = 1;
		break;

		case DIR_E:
			pos.x = 1;
			pos.y = 0;
		break;

		case DIR_NW:
			pos.x = -1;
			pos.y = -1;
		break;

		case DIR_NE:
			pos.x = 1;
			pos.y = -1;
		break;

		case DIR_SW:
			pos.x = -1;
			pos.y = 1;
		break;

		case DIR_SE:
			pos.x = 1;
			pos.y = 1;
		break;
		
		default: break;

	}

	return pos;
}

double
face_dir(vec2 dst, vec2 pos)
{
	// dst = mouse cursor on screen
	// pos = center hitbox - camera
	vec2 f = vec2_sub(dst, pos);
	double d = (atan2(f.y, f.x) * (180 / M_PI));
	//log_trace("x %d y %d a %f", f.x, f.y, d);
	d += 360;
	d += 22.5f;
	d = fmod(d, 360);

	return d;
}

enum direction
dir_from_mouse(vec2 mouse_pos, vec2 pos)
{
	double angle = face_dir(mouse_pos, pos);
	angle /= 45;

	angle = floor(angle);

	if (angle == 0) return DIR_E;
	if (angle == 1) return DIR_SE;
	if (angle == 2) return DIR_S;
	if (angle == 3) return DIR_SW;
	if (angle == 4) return DIR_W;
	if (angle == 5) return DIR_NW;
	if (angle == 6) return DIR_N;
	if (angle == 7) return DIR_NE;

	return DIR_NONE;
}

enum hero_action
hero_action_from_input(uint16_t input)
{
	enum hero_action action = HERO_ACTION_IDLE;

	bool up = input & FLAG_INPUT_UP;
	bool left = input & FLAG_INPUT_LEFT;
	bool down = input & FLAG_INPUT_DOWN;
	bool right = input & FLAG_INPUT_RIGHT;
	
	if (up || left || down || right) action = HERO_ACTION_WALK;	

	bool cast0 = input & FLAG_INPUT_CAST0;
	bool cast1 = input & FLAG_INPUT_CAST1;
	bool cast2 = input & FLAG_INPUT_CAST2;
	bool cast3 = input & FLAG_INPUT_CAST3;
	bool cast4 = input & FLAG_INPUT_CAST4;

	//log_trace("0 %d 1 %d 2 %d 3 %d 4 %d", cast0, cast1, cast2, cast3, cast4);

	if (cast0) action = HERO_ACTION_CAST_SPELL0;
	if (cast1) action = HERO_ACTION_CAST_SPELL1;
	if (cast2) action = HERO_ACTION_CAST_SPELL2;
	if (cast3) action = HERO_ACTION_CAST_SPELL3;
	if (cast4) action = HERO_ACTION_CAST_SPELL4;

	return action;
}

enum hero_action
hero_action_cast_from_slot(uint8_t slot)
{
	enum hero_action action = HERO_ACTION_CAST_SPELL0;

	if (slot == SPELL_SLOT0) action = HERO_ACTION_CAST_SPELL0;
	if (slot == SPELL_SLOT1) action = HERO_ACTION_CAST_SPELL1;
	if (slot == SPELL_SLOT2) action = HERO_ACTION_CAST_SPELL2;
	if (slot == SPELL_SLOT3) action = HERO_ACTION_CAST_SPELL3;
	if (slot == SPELL_SLOT4) action = HERO_ACTION_CAST_SPELL4;

	return action;
}


