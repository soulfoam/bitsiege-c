#include "engine.h"
#include "network/client.h"
#include "network/client_packet.h"
#include "delta_time.h"
#include "data/data_hero.h"
#include "graphics/data/data_render_hero.h"
#include "util/log.h"
#include "util/vec2.h"
#include "graphics/game/render.h"
#include "graphics/menu/menu.h"
#include "input.h"
#include "debug.h"
#include "util/game_util.h"
#include "paths.h"
#include "game/spell/spell.h"
#include "data/data_spell.h"

static struct engine en;

struct engine*
eng(void)
{
	return &en;
}

void
engine_init(void)
{
	int client_result = client_init();
	if (!client_result) log_warn("Could not connect to server.");

//	packet_write(request_account_register("username2", "test1234", "wow@wow.com"), client()->pack_buf);
	
	//log_trace("lol %d", sizeof(struct command_world_snapshot));
	log_trace("Enter username:");
	char username[21];
	scanf("%s", username);
  
	request_ping();
	request_account_login(username, "test1234");
	request_party_que();

//	request_ping();
//	request_account_login("username1", "test1234");
//	request_party_que();


	input_init();
	graphics_init();
	render_init();
	data_spell_init();
	data_hero_init();
	data_render_hero_init();
	menu_init();
	en.game = game_init();	
	camera_init();	

	en.local_hero = NULL;
	en.info.in_game = false;
	en.info.last_time = 0;
	en.info.start_time = 0;
	en.info.loops = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		en.info.room_members[i] = party_member_init(-1, "NULL", COLOR_WHITE);
	}
	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		en.info.party_members[i] = party_member_init(-1, "NULL", COLOR_WHITE);
	}
	en.acc_info.f_list = friend_list_init();	
	en.acc_info.id = UINT_MAX;

	render_world_init();

	log_info("Engine initialized.");
}

void
engine_start(void)
{	
#ifdef __EMSCRIPTEN__
	graphics_calculate_html(en.gfx);
	emscripten_set_resize_callback(0, en.gfx, true, graphics_resize_html);
	emscripten_set_main_loop_arg(engine_loop, eng, 0, 0);
#else
	while (1)
	{
		engine_loop();
		SDL_Delay(2);
	}
#endif

	SDL_SetWindowGrab(graphics()->window, true);
	en.info.start_time = SDL_GetTicks();

	log_info("Engine started.");
}

void
engine_destroy(void)
{
	client_destroy();

	graphics_destroy();
	render_destroy();
	game_destroy(en.game);	
	menu_destroy();
	
	IMG_Quit();
	SDL_Quit();

	exit(0);
}

void
engine_events(void)
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
			case SDL_QUIT:
				engine_destroy();
			break;	

			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					graphics_calculate_scale();
				}
				else if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				{

				}
			break;
		}	

		menu_input(&e);
		input_process(&e);
	}


	if (en.info.in_game)
	{
		struct renderable_hero *rh = en.local_hero;
		vec2 p = vec2_init(
				rh->hero->pos.x + (rh->hero->hitboxes[HERO_HITBOX_FULL].size.x / 2), 
				rh->hero->pos.y + (rh->hero->hitboxes[HERO_HITBOX_FULL].size.y / 2));
		
		input()->mouse_dir_prev = input()->mouse_dir;
		input_mouse_dir_set(p);
	}
	
}

void
engine_update()
{	
	if (en.info.in_game)
	{
		// This should go into a client "game manager" that handles single player updating 
		// and multiplayer
		struct renderable_hero *rh = en.local_hero;
		struct command_input ci = {rh->hero, input()->flags_input, input()->mouse_dir};
		command_init_input(game_command_init(en.game), &ci);
		/////////////

		input_update();
		game_update(en.game);
		camera_update();
		render_update();	

	}

	menu_update();
	debug_update();
}

void
engine_render()
{
	SDL_SetRenderDrawColor(graphics()->renderer, 0xAA, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(graphics()->renderer);
	
	if (en.info.in_game)
	{
		camera_render();	
		render_world();
		gui_render();
	}

	menu_render();

	debug_render();

	SDL_RenderPresent(graphics()->renderer);

	fps_calculate();
}

void 
engine_loop()
{	
	uint32_t now = SDL_GetTicks();

	client_read();

	if (now - en.info.last_time > TICK_PERIOD)
	{
		client_process();
		engine_update();
		client_write_packets();
		
		en.info.loops++;
		en.info.last_time += TICK_PERIOD;	
	}

	float percent = ((now - en.info.last_time) / TICK_PERIOD);
	R_PERC = percent;

	engine_render();
	engine_events();	

	delta_time_calculate();

	//log_trace("fps %u", FPS_FPS);
}


