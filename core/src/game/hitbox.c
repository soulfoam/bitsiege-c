#include "hitbox.h"
#include "../util/log.h"

struct hitbox
hitbox_init(vec2 pos_offset, vec2 size)
{
	struct hitbox hb = {vec2_init(0, 0), pos_offset, size};
	return hb;
}

bool
hitbox_collides(const struct hitbox *hb, const struct hitbox *hbo)
{

	if (hb->pos.x < hbo->pos.x + hbo->size.x && 
		hb->pos.x + hb->size.x > hbo->pos.x && 
		hb->pos.y < hbo->pos.y + hbo->size.y && 
		hb->size.y + hb->pos.y > hbo->pos.y)
	{
		return true;
	}

	return false;
}

void
hitbox_pos_set(struct hitbox *hb, vec2 pos)
{
	hb->pos = vec2_add(pos, hb->pos_offset);
}

void
hitbox_get_rect_size(
		const struct hitbox *hb, 
		int32_t *left, 
		int32_t *right, 
		int32_t *top, 
		int32_t *bottom)
{

	int32_t l = hb->pos.x;
	int32_t r = hb->pos.x + hb->size.x;
	int32_t t = hb->pos.y;
	int32_t b = hb->pos.y + hb->size.y;

	*left = l;
	*right = r;
	*top = t;
	*bottom = b;
}

