#include "../../engine.h"
#include "menu_main.h"
#include "util/log.h"
#include "../bitmap_font.h"
#include "util/util_funcs.h"

struct val day = {112, 215, 235, 255};
struct val sunset = {255, 147, 41, 255};
struct val night = {48, 69, 84, 255};
struct val sunrise = {253, 125, 1, 255};
struct val bloodmoon = {89, 39, 32, 255};

static uint64_t ticks = 0;

static struct menu_main* menu;

static struct bitmap_text *text_arr[TEXT_ARR_SIZE];

struct menu_main*
menu_main(void)
{
	return menu;
}

static struct time_cycle
time_cycle_init(uint8_t id, uint32_t duration, uint32_t lerp_duration, struct val color)
{
	struct time_cycle tc;
	tc.id = id;
	tc.duration = duration * TICKS_PER_SECOND;
	union color c;
	c.val = color;
	tc.lerp_color = c;
	tc.color = c;
	tc.time = 0;
	tc.start = 0;
	tc.lerp_duration = lerp_duration * TICKS_PER_SECOND;
	tc.switching = false;

	return tc;
}

static void
time_cycle_update_scene_colors(void)
{
	struct val c = menu->cur_cycle.lerp_color.val;

	uint8_t r = c.r;
	uint8_t g = c.g;
	uint8_t b = c.b;
	
	for (int i = 0; i < BG_LAYERS; i++)
	{	
		r = r * COLOR_MUL;
		g = g * COLOR_MUL;
		b = b * COLOR_MUL;
		SDL_SetTextureColorMod(menu->background[i].tex.texture, r, g, b);
	}
}

void
menu_main_init(void)
{
	
	menu = malloc(sizeof(struct menu_main));

	vec2 bg_size = {RES_WIDTH, RES_HEIGHT};
	struct SDL_Texture *bg_0 = texture_get("res/art/menu/main_bg_sun.png");
	struct SDL_Texture *bg_1 = texture_get("res/art/menu/main_bg_clouds.png");
	struct SDL_Texture *bg_1_1 = texture_get("res/art/menu/main_bg_clouds_back.png");
	struct SDL_Texture *bg_2 = texture_get("res/art/menu/main_bg_mountains_back.png");
	struct SDL_Texture *bg_3 = texture_get("res/art/menu/main_bg_mountains_front.png");
	struct SDL_Texture *bg_4 = texture_get("res/art/menu/main_bg_castle_1.png");
	struct SDL_Texture *bg_5 = texture_get("res/art/menu/main_bg_castle_2.png");
	struct SDL_Texture *bg_6 = texture_get("res/art/menu/main_bg_castle_3.png");
	struct SDL_Texture *bg_7 = texture_get("res/art/menu/main_bg_castle_4.png");
	struct SDL_Texture *bg_8 = texture_get("res/art/menu/main_bg_castle_5.png");
	struct SDL_Texture *bg_9 = texture_get("res/art/menu/main_bg_castle_6.png");

	struct SDL_Texture *logo = texture_get("res/art/menu/logo.png");

	menu->sword_timer_flash = 0;
	menu->start_timer = 0;

	menu->fade_color = color_init(0, 0, 0, 180);

	menu->pos_logo = vec2_init(168, 25);
	menu->pos_start_text = vec2_init(116, 210);

	struct animation anim = animation_init(0, 11, 100, false, 145);
	menu->logo = sprite_init(
			logo, 
			&anim, 
			menu->pos_logo,
			vec2_init(0, 0), 
			vec2_init(145, 115));	

	animation_play(&menu->logo.anim);	
	
	//good night time 25, 25, 112
	//blood red nice boi 89, 39, 32
	//day 112,215, 235
	//sunset is nice 255, 147, 41
	//do day and night for the users time of day? poggers
	//best night time IMO 59, 68, 75
	
	menu->cycles[0] = time_cycle_init(0, DURATION_DAY, TIME_DAY_TO_SUNSET, day);
	menu->cycles[1] = time_cycle_init(1, DURATION_SUNSET, TIME_SUNSET_TO_NIGHT, sunset);
	menu->cycles[2] = time_cycle_init(2, DURATION_NIGHT, TIME_NIGHT_TO_SUNRISE, night);
	menu->cycles[3] = time_cycle_init(3, DURATION_SUNRISE, TIME_SUNRISE_TO_DAY, sunrise);

	menu->cur_cycle = menu->cycles[0];

	menu->background[0] = sprite_init(bg_0, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	
	menu->background[1] = sprite_init(bg_1, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	
	menu->background[2] = sprite_init(bg_1_1, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	
	menu->background[3] = sprite_init(bg_2, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	
	menu->background[4] = sprite_init(bg_3, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	
	menu->background[5] = sprite_init(bg_4, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	
	menu->background[6] = sprite_init(bg_5, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	
	menu->background[7] = sprite_init(bg_6, NULL, vec2_init(0, 0), vec2_init(0, 0), bg_size);	


	struct animation a = animation_init(0, 4, 100, true, bg_size.x);

	menu->background[8] = sprite_init(bg_7, &a, vec2_init(0, 0), vec2_init(0, 0), bg_size);	

	animation_play(&menu->background[8].anim);

	menu->background[9] = sprite_init(bg_8, &a, vec2_init(0, 0), vec2_init(0, 0), bg_size);	

	animation_play(&menu->background[9].anim);

	menu->cycle_index = 0;	

	time_cycle_update_scene_colors();
//	menu->textbox_login_user_name = textbox_init(vec2_init(0, 0), vec2_init(100, 100));

	menu->pos_cloud = vec2_init(-50, 0);
	menu->pos_cloud_back = vec2_init(-50, 0);
	menu->pos_sun = vec2_init(0, 0);
	

	menu->pos_archer = vec2_init(80, 143);
	menu->archer_data = *data_render_hero_get(0);
	menu->archer_turn_around = false;
	menu->archer_data.spr.anim = 
		data_render_hero_get_anim(&menu->archer_data, HERO_ACTION_WALK, DIR_E);
		
	sprite_pos_set(&menu->archer_data.spr, menu->pos_archer);
	
	animation_play(&menu->archer_data.spr.anim);

	menu->pressed_start = false;
	menu->start_text_alpha = 0;
	menu->reset_blink = false;

	text_arr[0] = bitmap_text_init(
			"Press any button to continue...",
			menu->pos_start_text, 
			renderer()->bmp_font_title, 
			color_init_c(COLOR_WHITE));

	text_arr[1] = bitmap_text_init(
			"Solo",
			vec2_init(60, 254), 
			renderer()->bmp_font_title, 
			color_init_c(COLOR_WHITE));

	text_arr[2] = bitmap_text_init(
			"Solo",
			vec2_init(116, 210), 
			renderer()->bmp_font_title, 
			color_init_c(COLOR_WHITE));


	log_info("Main Menu initialized.");

}

void
menu_main_destroy(void)
{
	for (int i = 0; i < TEXT_ARR_SIZE; i++)
	{
		free(text_arr[i]);
	}
	free(menu);
}

uint8_t
time_cycle_add_index(void)
{
	uint8_t i = menu->cycle_index + 1;
	if (i >= CYCLE_COUNT)
	{
		i = 0;
	}

	return i;
}

static void
time_cycle_smooth_color_change(void)
{

	if (!menu->cur_cycle.switching) 
	{
		menu->cur_cycle.start = ticks;
		menu->cur_cycle.switching = true;
	}

	float delta = ticks - menu->cur_cycle.start;

	if (delta > 0.0f)
	{
		delta /= (float) menu->cur_cycle.lerp_duration;
		if (delta >= 1.0f) delta = 1.0f;
	}

	uint8_t next_index = time_cycle_add_index();

	union color src = menu->cur_cycle.color;	
	union color dst = menu->cycles[next_index].color;	
	
	menu->cur_cycle.lerp_color = lerp_rgb(src, dst, delta);

	time_cycle_update_scene_colors();
	
//	log_trace("%d %d %d", c.val.r, c.val.g, c.val.b);
//
//

}

static void
menu_main_update_clouds(void)
{

	menu->pos_cloud.x += CLOUD_SPEED;
	sprite_pos_set(&menu->background[1], menu->pos_cloud);
	if (menu->pos_cloud.x >= 510)
	{
		menu->pos_cloud.x = -460;
		sprite_pos_set_all(&menu->background[1], menu->pos_cloud);
	}


	sprite_pos_set(&menu->background[2], menu->pos_cloud_back);
	menu->pos_cloud_back.x += CLOUD_SPEED_BACK;
	if (menu->pos_cloud_back.x >= 510)
	{
		menu->pos_cloud_back.x = -464;
		sprite_pos_set_all(&menu->background[2], menu->pos_cloud_back);
	}


}

static void
menu_main_update_hero(void)
{

	if (menu->pos_archer.x >= 385)
	{
		menu->archer_turn_around = true;
	}
	if (menu->pos_archer.x <= 80)
	{
		menu->archer_turn_around = false;
	}

	if (!menu->archer_turn_around)
	{
		struct animation a = data_render_hero_get_anim(&menu->archer_data, HERO_ACTION_WALK, DIR_E);
		if (menu->archer_data.spr.anim.start_index != a.start_index)
		{
			menu->archer_data.spr.anim = a;
			animation_play(&menu->archer_data.spr.anim);
		}
		menu->pos_archer.x += HERO_SPEED;
	}
	else
	{
		struct animation a = data_render_hero_get_anim(&menu->archer_data, HERO_ACTION_WALK, DIR_W);
		if (menu->archer_data.spr.anim.start_index != a.start_index)
		{
			menu->archer_data.spr.anim = a;
			animation_play(&menu->archer_data.spr.anim);
		}
		menu->pos_archer.x -= HERO_SPEED;
	}

	sprite_pos_set(&menu->archer_data.spr, menu->pos_archer);


}

static void
menu_main_update_cycles(void)
{

	sprite_pos_set(&menu->background[0], menu->pos_sun);

	int32_t tl = menu->cur_cycle.duration - menu->cur_cycle.time;


	if (menu->cur_cycle.id == 0) // day
	{
		if (tl <= TIME_DAY_TO_SUNSET * TICKS_PER_SECOND)
		{
			menu->pos_sun.y += SUN_SPEED;

			time_cycle_smooth_color_change();
		}
	}
	else if (menu->cur_cycle.id == 1) // sunset
	{
		if (tl <= TIME_SUNSET_TO_NIGHT * TICKS_PER_SECOND)
		{
			menu->pos_sun.y += SUN_SPEED;

			time_cycle_smooth_color_change();
		}
	}
	else if (menu->cur_cycle.id == 2) // night
	{
		if (tl <= TIME_NIGHT_TO_SUNRISE * TICKS_PER_SECOND)
		{
			menu->pos_sun.y -= SUN_SPEED;

			time_cycle_smooth_color_change();
		}
	}
	else if (menu->cur_cycle.id == 3) // sunrise
	{
		if (tl <= TIME_SUNRISE_TO_DAY * TICKS_PER_SECOND)
		{
			menu->pos_sun.y -= SUN_SPEED;

			time_cycle_smooth_color_change();
		}
	}
	
	menu->cur_cycle.time++;
	if (menu->cur_cycle.time >= menu->cur_cycle.duration)
	{
		menu->cur_cycle.switching = false;
		menu->cycle_index = time_cycle_add_index();
		menu->cur_cycle = menu->cycles[menu->cycle_index];

		time_cycle_update_scene_colors();

	}
}

static void
menu_main_update_title_screen(void)
{
	menu->sword_timer_flash++;
	if (menu->sword_timer_flash >= SWORD_FLASH_INTERVAL)
	{
		animation_play(&menu->logo.anim);
		menu->sword_timer_flash = 0;
	}
	
	if (menu->start_timer > 0)
	{
		menu->pos_logo.y -= START_BUTTON_ANIM_SPEED;
		menu->start_timer--;

		menu->fade_color.val.a -= START_FADE_SPEED;
		menu->fade_color.val.a = clamp(menu->fade_color.val.a, 0, 255);
		
		menu->start_text_alpha = 255;
		menu->pos_start_text.y += START_FADE_TEXT_SPEED;

		if (menu->start_timer <= 0) menu->pressed_start = true;

		sprite_pos_set(&menu->logo, menu->pos_logo);
		bitmap_text_pos_set(text_arr[0], menu->pos_start_text);
	}
	else
	{

		if (menu->reset_blink)
		{
			menu->start_text_alpha -= TEXT_BLINK_SPEED;

			if (menu->start_text_alpha <= 0) 
			{
				menu->reset_blink = false;
				menu->start_text_alpha = 0;
			}
		}
		else
		{
			menu->start_text_alpha += TEXT_BLINK_SPEED;

			if (menu->start_text_alpha >= 255)
			{
				menu->reset_blink = true;
				menu->start_text_alpha = 255;
			}
		}

	}

}

void
menu_main_input(SDL_Event *e)
{
	switch (e->type)
	{
		case SDL_KEYDOWN:
			if (e->key.keysym.sym == SDLK_RETURN)
			{
				menu->start_timer = START_BUTTON_DELAY;
				menu->start_text_alpha = 255;
			}
		break;
	}
}

void
menu_main_update(void)
{
	if (!menu->pressed_start) menu_main_update_title_screen();

	menu_main_update_cycles();
	menu_main_update_clouds();
	menu_main_update_hero();

	ticks++;

}

static void
menu_main_render_title_screen(void)
{

	SDL_Rect bg_fill = {0, 0, WINDOW_SIZE.x, WINDOW_SIZE.y};

	if (menu->start_timer > 0)
	{
		struct val v = menu->fade_color.val;
		SDL_SetRenderDrawColor(graphics()->renderer, v.r, v.g, v.b, v.a);
		SDL_RenderFillRect(graphics()->renderer, &bg_fill);
	}
	else
	{
		SDL_SetRenderDrawColor(graphics()->renderer, 0, 0, 0, 180);
		SDL_RenderFillRect(graphics()->renderer, &bg_fill);
	}

	sprite_render(&menu->logo);
	
	bitmap_text_render_alpha(text_arr[0], menu->start_text_alpha);
	
}

void
menu_main_render_ui_bar(void)
{
//	int32_t h = 24;
//	SDL_Rect ui_bar = {0, (RES_HEIGHT - h) * GAME_SCALE.y, RES_WIDTH * GAME_SCALE.x, h * GAME_SCALE.y};
//	SDL_SetRenderDrawColor(graphics()->renderer, 0, 0, 0, 120);
//	SDL_RenderFillRect(graphics()->renderer, &ui_bar);
//	
//	bitmap_text_render(text_arr[1]);

//	bitmap_font_render(
//		renderer()->bmp_font_title,
//		"Solo",
//		color_init_c(COLOR_GREEN), 
//		vec2_init(60, 254));
//
//	bitmap_font_render(
//		renderer()->bmp_font,
//		"SoulfoamSoulfoamSoul",
//		color_init_c(COLOR_WHITE), 
//		vec2_init(130, 254));
}

void
menu_main_render(void)
{	
	struct val c = menu->cur_cycle.lerp_color.val;

	SDL_SetRenderDrawColor(graphics()->renderer, c.r, c.g, c.b, c.a);

	SDL_Rect bg_fill = {0, 0, WINDOW_SIZE.x, WINDOW_SIZE.y};
	SDL_RenderFillRect(graphics()->renderer, &bg_fill);

	
	for (int i = 0; i < BG_LAYERS; i++)
	{
		sprite_render(&menu->background[i]);

		if (i == 5)
		{

			SDL_SetTextureColorMod(menu->archer_data.spr.tex.texture, 0, 0, 0);
			SDL_SetTextureAlphaMod(menu->archer_data.spr.tex.texture, 100);

			sprite_render(&menu->archer_data.spr);

			SDL_SetTextureColorMod(menu->archer_data.spr.tex.texture, 255, 255, 255);
			SDL_SetTextureAlphaMod(menu->archer_data.spr.tex.texture, 255);
		}

	}

	if (!menu->pressed_start) 
	{
		menu_main_render_title_screen();
	}
	else
	{
		menu_main_render_ui_bar();
	}
	

}
