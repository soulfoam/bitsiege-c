#ifndef MENU_MAIN__
#define MENU_MAIN__

#include "../graphics.h"
#include "../textbox.h"
#include "../../graphics/data/data_render_hero.h"
#include "../../graphics/game/render.h"

#define BG_LAYERS 10

#define CLOUD_SPEED 0.6f
#define CLOUD_SPEED_BACK 0.45f
#define HERO_SPEED 2
#define SUN_SPEED 0.35f

#define COLOR_MUL 0.91f

#define TEXT_BLINK_SPEED 12

#define SWORD_FLASH_INTERVAL 4 * TICKS_PER_SECOND

#define START_BUTTON_ANIM_SPEED 15
#define START_BUTTON_DELAY 16
#define START_FADE_SPEED 12
#define START_FADE_TEXT_SPEED 6

#define CYCLE_COUNT 4
#define TIME_DAY_TO_SUNSET 12
#define TIME_SUNSET_TO_NIGHT 12
#define TIME_NIGHT_TO_SUNRISE 12
#define TIME_SUNRISE_TO_DAY 12

#define DURATION_DAY 22
#define DURATION_SUNSET 22
#define DURATION_NIGHT 22
#define DURATION_SUNRISE 22

#define TEXT_ARR_SIZE 10

struct
time_cycle
{
	uint8_t id;
	float duration;
	float time;
	union color color;

	union color lerp_color;
	int32_t lerp_duration;
	int32_t start;
	bool switching;

};

struct 
menu_main
{
	struct textbox *textbox_login_user_name;
	struct textbox *textbox_login_password;

	struct sprite logo;
	struct sprite background[BG_LAYERS];
	
	struct data_render_hero archer_data;
	vec2 pos_archer;
	bool archer_turn_around;

	struct time_cycle cycles[CYCLE_COUNT];
	struct time_cycle cur_cycle;
	uint8_t cycle_index;

	bool pressed_start;
	int16_t start_text_alpha;
	bool reset_blink;

	float sword_timer_flash;	
	float start_timer;

	vec2 pos_cloud;
	vec2 pos_cloud_back;
	vec2 pos_sun;
	
	vec2 pos_logo;
	vec2 pos_start_text;

	union color fade_color;


};

struct menu_main*
menu_main(void);

void
menu_main_init(void);

void
menu_main_destroy(void);

void
menu_main_input(SDL_Event *e);

void
menu_main_update(void);

void
menu_main_render(void);

#endif


