#include "animation.h"
#include "../../delta_time.h"
#include "util/log.h"
#include "util/game_values.h"

struct animation
animation_init(
		uint16_t start_index,
		uint16_t num_of_frames,
		uint16_t delay,
		bool loop,
		uint32_t frame_width)
{

	struct animation anim;

	anim.playing = false;
	anim.timer = 0;

	anim.loop = loop;
	anim.delay = delay;
	anim.start_index = start_index;
	anim.current_frame_num = anim.start_index-1;
	anim.num_of_frames = num_of_frames;
	anim.frame_width = frame_width;
	anim.played_once = false;

	anim.src.x = 0;
	anim.src.y = 0;
	anim.src.w = anim.frame_width;
	anim.src.h = anim.frame_width;
	
	return anim;
}

void
animation_play(struct animation *anim)
{
	anim->playing = true;	
}

void
animation_play_from_frame(struct animation *anim, uint16_t frame_num)
{
	anim->current_frame_num = frame_num; 
	anim->playing = true;	
}

void
animation_play_from_animation(struct animation *anim, struct animation old)
{
	uint16_t of = old.current_frame_num - old.start_index;
	uint16_t new_frame_num = of + anim->start_index;
	if (new_frame_num >= anim->start_index + anim->num_of_frames) 
		new_frame_num = anim->start_index;

	anim->timer = old.timer;
	anim->current_frame_num = new_frame_num;
	anim->playing = true; 
}

void
animation_restart(struct animation *anim)
{
	anim->current_frame_num = anim->start_index;
	anim->timer = 0;
}

void
animation_stop(struct animation *anim)
{
	anim->playing = false;
	animation_restart(anim);
}

void
animation_render_update(struct animation *anim)
{
	if (anim->playing)
	{
		anim->timer += DELTA_TIME;

		if (anim->timer > anim->delay)
		{
			anim->current_frame_num++;
			if (anim->current_frame_num == anim->start_index + anim->num_of_frames)
			{
				anim->played_once = true;

				if (anim->loop)
				{
					anim->current_frame_num = anim->start_index;
				}
				else
				{
					animation_stop(anim);
					return;
				}
			}

			anim->timer = 0;
		}

		anim->src.x = anim->frame_width * anim->current_frame_num;
		anim->src.y = 0;
		anim->src.w = anim->frame_width;
		anim->src.h = anim->frame_width;

	}
}

