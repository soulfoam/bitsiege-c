#include "gui.h"
#include "util/log.h"
#include "texture.h"

void
gui_render_hud(void)
{
	
}

void
gui_render_input(void)
{	

}

void
gui_render(void)
{

	//struct camera *cam = &world->local_player->cam;

//	vec2 scale = {gfx->scale.x, gfx->scale.y};
//	SDL_Rect dst;
//
//	dst.x = floor(20 * scale.x - cam->pos.x * scale.x);
//	dst.y = floor(20 * scale.y - cam->pos.y * scale.y);
//	dst.w = ceil(64 * scale.x);
//	dst.h = ceil(69 * scale.y);
//
//	SDL_RenderCopy(
//			gfx->renderer, 
//			texture_get(TEXTURE_CASTLE),
//			NULL, 
//			&dst);
	
	gui_render_input();	
}

