#include "../../engine.h"
#include "menu.h"
#include "util/log.h"

static struct menu *mn;

struct menu*
menu(void)
{
	return mn;
}

void
menu_init(void)
{
	mn = malloc(sizeof(struct menu));

	mn->state = MENU_STATE_MAIN;

	struct SDL_Texture *transition = texture_get("res/art/menu/transition.png");
	mn->transition = sprite_init(
			transition, NULL, 
			vec2_init(0, 0), 
			vec2_init(0, 0),
			vec2_init(RES_WIDTH, RES_HEIGHT));

	mn->pos_trans = vec2_init(0, 0);
	mn->transitioning = false;

	menu_main_init();

	log_info("Menu initialized.");
}

void
menu_destroy(void)
{
	menu_main_destroy();
	free(mn);
}

void
menu_state_set(enum menu_state state)
{
	if (mn->state != state)
	{
		mn->state = state;
		vec2 p = vec2_init(0, 0);
		
		mn->pos_trans = p;
		sprite_pos_set(&mn->transition, p);
		
		mn->transitioning = true;
	}
}

void
menu_input(SDL_Event *e)
{
	menu_main_input(e);
}

void
menu_update(void)
{
	if (mn->transitioning)
	{
		mn->pos_trans.x += TRANSITION_SPEED;
		if (mn->pos_trans.x >= 800) mn->transitioning = false;
	}

	sprite_pos_set(&mn->transition, mn->pos_trans);


	if (mn->state == MENU_STATE_PLAY) return;


	menu_main_update();

} 

void
menu_render(void)
{
	
	if (mn->state == MENU_STATE_PLAY)
	{

	}
	else
	{
		menu_main_render();
	}

	if (mn->transitioning) sprite_render(&mn->transition);

}

