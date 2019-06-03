#ifndef PATHFINDING__
#define PATHFINDING__

#include "tile_grid.h"
#include "../game/hitbox.h"
#include "../util/s_list.h"

struct
pf_path
{
	struct s_list_node *path;
	uint32_t path_size;
};

struct
pf_settings
{
	vec2 unit_size;
	bool collide;
};

void
pf_path_destroy(struct pf_path *path);

struct pf_path*
pf_path_find(
		struct tile_grid *grid, 
		struct tile *start, 
		struct tile *end, 
		struct pf_settings settings);

#endif



