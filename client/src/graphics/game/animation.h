#ifndef ANIMATION__
#define ANIMATION__

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "texture.h"
#include "game/direction.h"

struct
animation
{
	bool playing;
	bool loop;
	uint16_t start_index;
	uint16_t num_of_frames;
	uint16_t delay;
	uint32_t frame_width;

	uint16_t current_frame_num;
	uint32_t timer;
	bool played_once;

	SDL_Rect src;	
};

struct animation
animation_init(
		uint16_t start_index,
		uint16_t num_of_frames,
		uint16_t delay,
		bool loop,
		uint32_t frame_width);

void
animation_play(struct animation *anim);

void
animation_play_from_frame(struct animation *anim, uint16_t frame_num);

void
animation_play_from_animation(struct animation *anim, struct animation old);

void
animation_stop(struct animation *anim);

void
animation_restart(struct animation *anim);

void
animation_render_update(struct animation *anim);

#endif
