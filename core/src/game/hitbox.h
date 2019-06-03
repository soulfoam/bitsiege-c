#ifndef HITBOX__
#define HITBOX__

#include <inttypes.h>
#include <stdbool.h>
#include "../util/vec2.h"
#include "../util/util_funcs.h"

struct
hitbox
{
	vec2 pos;
	vec2 pos_offset;
	vec2 size;
};

struct hitbox
hitbox_init(vec2 pos_offset, vec2 size);

bool
hitbox_collides(const struct hitbox *hb, const struct hitbox *hbo);

void
hitbox_pos_set(struct hitbox *hb, vec2 pos);

void
hitbox_get_rect_size(
		const struct hitbox *hb, 
		int32_t *left, 
		int32_t *right, 
		int32_t *top, 
		int32_t *bottom);

#endif


