#ifndef DEBUG__
#define DEBUG__

#include <stdbool.h>

struct
debug_values
{
	bool hitbox_show;	
	bool camera_trace;	
};

const struct debug_values*
debug_values(void);

void
debug_camera_trace(void);

void
debug_hitbox(void);

void
debug_update(void);

void
debug_render(void);


#endif


