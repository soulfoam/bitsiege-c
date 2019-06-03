#ifndef CAMERA__
#define CAMERA__

#include "util/vec2.h"
#include "../../input.h"
#include "../game/renderable_hero.h"
#include "game/hero/hero.h"

#define CAMERA_SPEED 0.25

struct
camera
{
	vec2 offset;
	vec2 pos_prev;
	vec2 pos;
	vec2 pos_view;

	float speed;
	bool locked;

	bool view_follow_hero;

	const struct renderable_hero *follow_hero;
};

struct camera*
camera(void);

void
camera_init(void);

void
camera_update(void);

void
camera_render(void);

vec2
camera_pos(void);

void
camera_follow_hero(const struct renderable_hero *ren_hero, const vec2 offset);


#endif

