#include <math.h>
#include "camera.h"
#include "../../engine.h"
#include "util/log.h"
#include "../../delta_time.h"

static struct camera cam;

struct camera*
camera(void)
{
	return &cam;
}

void
camera_init(void)
{
	
	cam.offset = vec2_init(0, 0);
	cam.pos = vec2_init(0, 0);
	cam.pos_prev = vec2_init(0, 0);
	cam.pos_view = vec2_init(0, 0);

	cam.speed = 8;

	cam.follow_hero = NULL;
	cam.locked = true;
	cam.view_follow_hero = false;

	log_info("Camera initialized.");
}

static void
camera_keep_in_bounds(vec2 *pos)
{
	//vec2 b = vec2_init(230, 0);
	vec2 b = vec2_init(0, 0);

	if (pos->x <= b.x) pos->x = b.x;
	if (pos->y <= b.y) pos->y = b.y;
	if (pos->x >= eng()->game->world.size.x) pos->x = eng()->game->world.size.x;
	if (pos->y >= eng()->game->world.size.y) pos->y = eng()->game->world.size.y;

}

static vec2
camera_move_vec(void)
{
	vec2 p = input_mouse_pos_screen();
	
	uint8_t input = 0;
	vec2 trigger = vec2_init(8, 6);

	if (p.y <= trigger.y) input |= FLAG_INPUT_UP;
	if (p.x <= trigger.x) input |= FLAG_INPUT_LEFT; 
	if (p.y >= RES_HEIGHT - trigger.y) input |= FLAG_INPUT_DOWN; 
	if (p.x >= RES_WIDTH - trigger.x) input |= FLAG_INPUT_RIGHT;	

	return vec2_from_dir(move_dir_from_input(input));
}

static void
camera_pos_hero_set(void)
{
	if(cam.follow_hero == NULL) return;

	cam.pos_prev = cam.follow_hero->hero->ss->pos;
	cam.pos = cam.follow_hero->hero->pos;
}

void
camera_update(void)
{
	cam.pos_prev = cam.pos;

	if (!cam.locked)
	{
		vec2 move = camera_move_vec();
		move = vec2_mul(move, cam.speed);
							
		cam.pos = vec2_add(cam.pos, move);	
	
		if (cam.view_follow_hero) camera_pos_hero_set();

		vec2 offset_abs = vec2_init(fabs(cam.offset.x), fabs(cam.offset.y));
		if (cam.pos.x <= offset_abs.x) cam.pos.x = offset_abs.x;
		if (cam.pos.y <= offset_abs.y) cam.pos.y = offset_abs.y;

	}
	else 
	{	
		camera_pos_hero_set();
	}

	camera_keep_in_bounds(&cam.pos);


}

void
camera_render(void)
{	
	cam.pos_view = vec2_lerp(cam.pos_prev, cam.pos, R_PERC);	 
}

vec2
camera_pos(void)
{
	vec2 pos = vec2_add(cam.pos_view, cam.offset); 

	camera_keep_in_bounds(&pos);
	
	return pos;
}

void
camera_follow_hero(const struct renderable_hero *ren_hero, const vec2 offset)
{
	cam.pos = ren_hero->hero->pos;
	cam.pos_prev = ren_hero->hero->ss->pos;
	cam.pos_view = ren_hero->hero->ss->pos;
	cam.follow_hero = ren_hero;
	cam.offset = offset;
}



