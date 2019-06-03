#include "input.h"
#include "network/client.h"
#include "network/client_packet.h"
#include "util/log.h"
#include "graphics/graphics.h"
#include "util/game_util.h"
#include "debug.h"

static struct input in;

struct input*
input(void)
{
	return &in;
}

void
input_init(void)
{
	in.flags_input_prev = 0;
	in.flags_input = 0;
	in.flags_game = 0;
	in.mouse_click_hold = false;
	in.mouse_dir = DIR_N;
	in.mouse_dir_prev = DIR_N;
}

vec2
input_mouse_pos_screen(void)
{
	int32_t x = 0;
	int32_t y = 0;
	SDL_GetMouseState(&x, &y);
	x /= GAME_SCALE.x;
	y /= GAME_SCALE.y;

	return vec2_init((float)x, (float)y);
}

vec2
input_mouse_pos_world(void)
{
	vec2 pos = input_mouse_pos_screen();
	pos.x = (pos.x / GAME_SCALE.x) + ((int32_t) camera_pos().x);	
	pos.y = (pos.y / GAME_SCALE.y) + ((int32_t) camera_pos().y);	
	if (pos.x <= 0) pos.x = 0;
	if (pos.y <= 0) pos.y = 0;
	if (pos.x >= eng()->game->world.size.x) pos.x = eng()->game->world.size.x;
	if (pos.y >= eng()->game->world.size.y) pos.y = eng()->game->world.size.y;
	return pos;
}

void
input_mouse_dir_set(vec2 pos)
{
	vec2 cam = camera_pos();
	vec2 p = vec2_sub(pos, cam);

	vec2 m = input_mouse_pos_screen();

	in.mouse_dir = dir_from_mouse(m, p);
}

void
input_update(void)
{
	
	uint8_t slot = UCHAR_MAX;

	if (in.flags_input & FLAG_INPUT_CAST0) slot = SPELL_SLOT0;
	if (in.flags_input & FLAG_INPUT_CAST1) slot = SPELL_SLOT1;
	if (in.flags_input & FLAG_INPUT_CAST2) slot = SPELL_SLOT2;
	if (in.flags_input & FLAG_INPUT_CAST3) slot = SPELL_SLOT3;
	if (in.flags_input & FLAG_INPUT_CAST4) slot = SPELL_SLOT4;

	if (slot != UCHAR_MAX) 
	{
		struct hero *target = NULL;	
		target = world_hero_get_at(&eng()->game->world, input_mouse_pos_world());
		uint32_t id_target = UINT_MAX;
		if (target != NULL) id_target = target->id_game;
		
		log_trace("target: %d", id_target);
		request_spell_cast(slot, id_target);
	}

	//if (in.flags_input_prev != in.flags_input 
	//		|| in.mouse_dir != in.mouse_dir_prev)
	//{

		request_input_update(in.flags_input, in.mouse_dir);
	//}
	


	in.flags_input_prev = in.flags_input;
}

static void
input_key_down(SDL_Event *e)
{
	uint16_t *i = &in.flags_input;
	uint8_t *gf = &in.flags_game;

	const uint8_t *k = SDL_GetKeyboardState(NULL);

	switch (e->key.keysym.sym)
	{
		case SDLK_p:
			debug_hitbox();	
		break;

		case SDLK_c:
			camera()->locked = !camera()->locked;
		break;

		case SDLK_v:
			SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		break;

		case SDLK_ESCAPE:
			SDL_SetWindowGrab(graphics()->window, !SDL_GetWindowGrab(graphics()->window));
		break;

		case SDLK_k:
			debug_camera_trace();
		break;

		case SDLK_g:
			SDL_SetWindowFullscreen(graphics()->window, SDL_WINDOW_FULLSCREEN);
			graphics_calculate_scale();
		break;

	}
}

void
input_process(SDL_Event *e)
{
	
	const uint8_t *k = SDL_GetKeyboardState(NULL);

	uint16_t *inf = &in.flags_input;

	*inf &= ~(FLAG_INPUT_UP | FLAG_INPUT_LEFT | FLAG_INPUT_DOWN | FLAG_INPUT_RIGHT);

	if (k[SDL_SCANCODE_W]) *inf |= FLAG_INPUT_UP;
	if (k[SDL_SCANCODE_A]) *inf |= FLAG_INPUT_LEFT; 
	if (k[SDL_SCANCODE_S]) *inf |= FLAG_INPUT_DOWN; 
	if (k[SDL_SCANCODE_D]) *inf |= FLAG_INPUT_RIGHT;	
	if (k[SDL_SCANCODE_1]) *inf |= FLAG_INPUT_CAST1;	
	if (k[SDL_SCANCODE_2]) *inf |= FLAG_INPUT_CAST2;	
	if (k[SDL_SCANCODE_3]) *inf |= FLAG_INPUT_CAST3;	
	if (k[SDL_SCANCODE_4]) *inf |= FLAG_INPUT_CAST4; 	
	
	camera()->view_follow_hero = false;
	if (k[SDL_SCANCODE_SPACE] || k[SDL_SCANCODE_TAB]) camera()->view_follow_hero = true;

	switch (e->type)
	{
		case SDL_KEYDOWN:
			input_key_down(e);
		break;

		case SDL_MOUSEBUTTONDOWN:		
			if (e->button.button == SDL_BUTTON_LEFT) *inf |= FLAG_INPUT_CAST0;	
		break;

		case SDL_MOUSEBUTTONUP:		
			if (e->button.button == SDL_BUTTON_LEFT) *inf &= ~(FLAG_INPUT_CAST0);
		break;

	}

}


