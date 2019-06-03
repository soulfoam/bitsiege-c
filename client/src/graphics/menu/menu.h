#ifndef MENU__
#define MENU__

#include "menu_main.h"
#include "../graphics.h"
#include "../textbox.h"

#define TRANSITION_SPEED 35

enum
menu_state
{
	MENU_STATE_MAIN,
	MENU_STATE_LOBBY,
	MENU_STATE_PLAY
};

struct
menu
{
	struct menu_main menu_main;
	enum menu_state state;
	
	struct sprite transition;	
	vec2 pos_trans;
	bool transitioning;

};

void
menu_init(void);

void
menu_destroy(void);

void
menu_state_set(enum menu_state state);

void
menu_input(SDL_Event *e);

void
menu_update(void);

void
menu_render(void);

#endif


