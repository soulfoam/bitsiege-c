#include <limits.h>
#include <math.h>
#include "pathfinding.h"
#include "../game/direction.h"
#include "../util/log.h"

static uint32_t
pf_heuristic_value(struct tile *tile, struct tile *end)
{
	return 2 * (abs((int32_t)tile->pos.x - (int32_t)end->pos.x) + abs((int32_t)tile->pos.y - (int32_t)end->pos.y));
}

static void
pf_add_tile(struct tile **list, struct tile *tile)
{
	if (*list) tile->next = *list;

	*list = tile;
}

static struct tile*
pf_get_min_f(struct tile **list)
{
	struct tile *min = NULL;
	struct tile *cur = *list;
	
	while (cur != NULL)
	{
		if (!cur->checked)
		{
			if (min == NULL)
			{
				min = cur;
			}
			else
			{
				if (cur->f < min->f) min = cur;
			}
		}
		cur = cur->next;
	}

	if (min != NULL) min->checked = true;
	
	return min;
}

void
pf_set_path_values(
		struct tile **visited_list,
		struct tile *t, 
		struct tile *cur, 
		struct tile *end, 
		int32_t *g, 
		int32_t *h, 
		int32_t *f)
{
	if (!t->checked)
	{
		*g = cur->g + 1;	
		*h = pf_heuristic_value(t, end);
		*f = *g + *h;

		if (t->f > *f)
		{
			if (t->f == INT_MAX) pf_add_tile(visited_list, t);

			t->g = *g;
			t->h = *h;
			t->f = *f;
			t->parent = cur;
		}
	}
}

bool
pf_diagonal_blocked(struct tile_neighbors_list *n, int32_t i)
{

	bool blocked = false;
	if (i == DIR_NW)
	{
		if (n->n[DIR_N]->blocked || n->n[DIR_W]->blocked) 
		blocked = true;
	}
	if (i == DIR_NE)
	{
		if (n->n[DIR_N]->blocked || n->n[DIR_E]->blocked) 
		blocked = true;
	}
	if (i == DIR_SW)
	{
		if (n->n[DIR_S]->blocked || n->n[DIR_W]->blocked)
		blocked = true;
	}
	if (i == DIR_SE)
	{
		if (n->n[DIR_S]->blocked || n->n[DIR_E]->blocked) 
		blocked = true;
	}

	return blocked;
}

void
pf_path_destroy(struct pf_path *path)
{	
	s_list_clear(&path->path);	
	free(path);
}

struct pf_path*
pf_path_find(
		struct tile_grid *grid, 
		struct tile *start, 
		struct tile *end, 
		struct pf_settings settings)
{

	//log_trace("Requesting path to %d %d from %d %d", end->pos.x, end->y, start->x, start->y);

	if (end->blocked || start == end) return NULL;

	int32_t h = 0;
	int32_t g = 0;
	int32_t f = 0;
	
	int32_t node_visted_count = 0;

	bool path_found = false;

	struct tile *visited_list = NULL;
	pf_add_tile(&visited_list, start);

	while (visited_list != NULL)
	{
		struct tile *cur = pf_get_min_f(&visited_list);

		if (cur == NULL) break;

		if (cur == end)
		{
			path_found = true;
			break;
		}	

		node_visted_count++;

		// if the tile is surrounded, its unreachable, 
		// without this check we will infi-lock the game
		// we have other checks like this, but this one is simple and optimized
		struct tile_neighbors_list end_n = tile_neighbors(grid, end);
		bool open_tile = false;
		for (uint8_t i = 0; i < NEIGHBOR_AMOUNT/2; i++)
		{
			if (!end_n.n[i]->blocked && end_n.n[i] != end) open_tile = true;
		}
		if (!open_tile) break;


		struct tile_neighbors_list n = tile_neighbors(grid, cur);

		if (settings.collide)
		{			
			for (uint8_t i = 0; i < NEIGHBOR_AMOUNT; i++)
			{
				struct tile *t = n.n[i];
				if (!t->blocked && !pf_diagonal_blocked(&n, i))
				{
					pf_set_path_values(&visited_list, t, cur, end, &g, &h, &f);
				}
			}

			for (int y = 0; y <= settings.unit_size.y; y++)
			{
				for (int x = 0; x <= settings.unit_size.x; x++)
				{
					
					struct tile *n_t = tile_get(grid, vec2_init(cur->pos.x + x, cur->pos.y + y));
					struct tile_neighbors_list n = tile_neighbors(grid, n_t);
					for (uint8_t i = 0; i < NEIGHBOR_AMOUNT; i++)
					{
						struct tile *t = n.n[i];
						if (!t->blocked && !pf_diagonal_blocked(&n, i))
						{
							pf_set_path_values(&visited_list, t, cur, end, &g, &h, &f);
						}
					}	
				}
			}	
		}
		else
		{	
			for (uint8_t i = 0; i < NEIGHBOR_AMOUNT; i++)
			{
				struct tile *t = n.n[i];
				pf_set_path_values(&visited_list, t, cur, end, &g, &h, &f);
			}
		}
		

		if (node_visted_count > (grid->size.x * 5))
		{
			//search is too long, it had to of been unfindable
			break;
		}
	}

	if (path_found)
	{

		struct pf_path *path_result = malloc(sizeof(struct pf_path));
		path_result->path = NULL;

		uint32_t path_count = 0;

		while (end)
		{
			path_count++;

			s_list_add(&path_result->path, end); 

			end = end->parent;
		}

		path_result->path_size = path_count;

		struct tile *t = visited_list;
		while (t != NULL)
		{
			struct tile *next = t->next;
			tile_reset(t);
			t = next;
		}
		
		s_list_reverse(&path_result->path);	
		
		return path_result;
	}
	
	log_warn("Pathfinding had no path to %d %d from %d %d", end->pos.x, end->pos.y, start->pos.x, start->pos.y);

	return NULL;
}




