#ifndef ENGINE__
#define ENGINE__

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "game/world.h"
#include "game/game.h"
#include "graphics/graphics.h"
#include "graphics/game/gui.h"
#include "graphics/menu/menu.h"
#include "graphics/game/render.h"
#include "network/friend.h"
#include "network/party.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

struct
account_info
{
	struct friend_list f_list;
	uint32_t id;
};

struct 
engine_info
{
	uint32_t loops;
	float start_time;
	double last_time;
	uint32_t tick_count;
	bool can_tick;
	
	bool in_game;
	struct party_member room_members[MAX_PLAYERS];
	struct party_member party_members[MAX_PARTY_SIZE];
};

struct 
engine
{
	struct engine_info info;	

	struct account_info acc_info;

	struct game *game;
	struct renderable_hero *local_hero;

};

struct engine*
eng(void);

void
engine_init(void);

void
engine_start(void);

void 
engine_destroy(void);

void
engine_loop(void);


#endif

