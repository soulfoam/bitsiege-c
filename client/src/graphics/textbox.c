#include "textbox.h"
#include "util/log.h"
#include "../debug.h"

struct textbox*
textbox_init(const struct graphics *gfx, vec2 pos, vec2 size)
{
	struct textbox *textbox = malloc(sizeof(struct textbox));

	textbox->pos = pos;
	textbox->size = size;
	textbox->text = "oh yeah fuck";

	SDL_Color color = {255, 255, 255, 255};
	textbox->color_text = color;

	return textbox;
}

void
textbox_destroy(struct textbox *textbox)
{
	free(textbox);
}

void
textbox_render(struct textbox *textbox, const struct graphics *gfx)
{


}

void
textbox_set_text_size(struct textbox *textbox, uint16_t size)
{	

}

