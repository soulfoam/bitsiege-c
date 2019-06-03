#include <time.h>
#include <math.h>
#include <limits.h>
#include "tile_grid.h"
#include "../util/log.h"
#include "../game/direction.h"

struct tile_grid
tile_grid_init(vec2 grid_size, vec2 tile_size)
{

	struct tile_grid grid;
	grid.tiles = malloc(sizeof(struct tile) * (grid_size.x * grid_size.y));
	grid.size = grid_size;	
	grid.tile_size = tile_size;

	uint32_t id = 0;

	for (int32_t y = 0; y < grid.size.y; y++)
	{
		for (int32_t x = 0; x < grid.size.x; x++)
		{
			struct tile *t = tile_get(&grid, vec2_init(x, y));			
			
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				t->heroes[i] = NULL;
			}

			t->id = id;
			t->pos.x = x;
			t->pos.y = y;
			t->light = 150;
			t->blocked = false;
		//	if (rand() % 100 < 3) 
		//	t->blocked_ground = true;

			tile_reset(t);

			id++;
		}
	}

	log_info("Tile grid [%.0f][%.0f] initialized.", grid_size.x, grid_size.y);

	return grid;
}

void
tile_grid_destroy(struct tile_grid *grid)
{
	free(grid->tiles);
}

void
tile_reset(struct tile *tile)
{
	tile->parent = NULL;
	tile->next = NULL;
	tile->checked = false;
	tile->h = 0;
	tile->g = 0;
	tile->f = INT_MAX;	
}

void
tile_grid_clear(struct tile_grid *grid)
{

	for (int32_t y = 0; y < grid->size.y; y++)
	{
		for (int32_t x = 0; x < grid->size.x; x++)
		{
			struct tile *t = tile_get(grid, vec2_init(x, y));			
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				t->heroes[i] = NULL;
			}
		}
	}

}

void
tile_grid_reset(struct tile_grid *grid)
{
	for (int32_t y = 0; y < grid->size.y; y++)
	{
		for (int32_t x = 0; x < grid->size.x; x++)
		{
			struct tile *t = tile_get(grid, vec2_init(x, y));			
			
			t->blocked = false;
			

			t->parent = NULL;
			t->next = NULL;
			t->checked = false;
			t->h = 0;
			t->g = 0;
			t->f = INT_MAX;	

		}
	}
}

void
tile_grid_update(struct tile_grid *grid, struct hero *heroes)
{
	tile_grid_clear(grid);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct hero *hero = &heroes[i];
		if (hero->id_game == UINT_MAX) continue;

		struct tile *t = tile_get_f(grid, hero->pos);
		tile_hero_add(t, hero);

	}
}

void
tile_hero_add(struct tile *t, struct hero *hero)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (t->heroes[i] == NULL) 
		{
			t->heroes[i] = hero;
			t->hero_count++;
			break;
		}
	}
}

void
tile_hero_remove(struct tile *t, struct hero *hero)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (t->heroes[i] == NULL) continue;

		if (t->heroes[i]->id_game == hero->id_game)
		{
			t->heroes[i] = NULL;
			t->hero_count--;
			break;
		}
	}
}

struct heroes_in_range
tile_get_heroes_in_range(struct tile_grid *grid, const struct hero *hero)
{	

	struct heroes_in_range hir;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		hir.heroes_seen[i] = NULL;
		hir.heroes_seen_prev[i] = NULL;
	}
	
	uint8_t amount = 9;

	struct tile *tiles[amount];
	struct tile *t = tile_get_f(grid, hero->pos);	
	
	struct tile *n = tile_get(grid, vec2_init(t->pos.x, t->pos.y - 1));	
	struct tile *ne = tile_get(grid, vec2_init(t->pos.x + 1, t->pos.y - 1));	
	struct tile *e = tile_get(grid, vec2_init(t->pos.x + 1, t->pos.y));	
	struct tile *se = tile_get(grid, vec2_init(t->pos.x + 1, t->pos.y + 1));	
	struct tile *s = tile_get(grid, vec2_init(t->pos.x, t->pos.y + 1));	
	struct tile *sw = tile_get(grid, vec2_init(t->pos.x - 1, t->pos.y + 1));	
	struct tile *w = tile_get(grid, vec2_init(t->pos.x - 1, t->pos.y));	
	struct tile *nw = tile_get(grid, vec2_init(t->pos.x - 1, t->pos.y - 1));

	tiles[0] = t;
	tiles[1] = n;
	tiles[2] = ne;
	tiles[3] = e;
	tiles[4] = se;
	tiles[5] = s;
	tiles[6] = sw;
	tiles[7] = w;
	tiles[8] = nw;

	for (int i = 0; i < amount; i++)
	{
		struct tile *t = tiles[i];
		for (int j = 0; j < MAX_PLAYERS; j++)
		{
			struct hero *h = t->heroes[j];
			if (h == NULL) continue;
			
			if (h->id_game != hero->id_game) hir.heroes_seen[j] = h;	
		}
	}

	return hir;
}

void
tile_print(const struct tile *tile, const char *name)
{
	log_trace(
			"%s: [%.0f, %.0f] || ID: %d || Light: %d", 
			name, 
			tile->pos.x, 
			tile->pos.y, 
			tile->id, 
			tile->light
			);
}

struct tile*
tile_get(const struct tile_grid *grid, vec2 pos)
{
	float x = pos.x;
	float y = pos.y;

	if (x <= 0) x = 0;
	if (y <= 0) y = 0;
	if (x >= (grid->size.x)) x = grid->size.x-1;
	if (y >= (grid->size.y)) y = grid->size.y-1;

	return &grid->tiles[(uint32_t)(y * grid->size.x + x)];
}

struct tile*
tile_get_f(const struct tile_grid *grid, vec2 pos)
{

	float x = pos.x;
	float y = pos.y;

	if (x != 0) x /= grid->tile_size.x;
	if (y != 0) y /= grid->tile_size.y;
	
	return tile_get(grid, vec2_init(x, y));
}

void
tile_set(struct tile *old_tile, const struct tile *new_tile)
{
	*old_tile = *new_tile; 
}

struct tile_neighbors_list
tile_neighbors(const struct tile_grid *grid, const struct tile *tile)
{
	
	struct tile_neighbors_list n;

	n.n[DIR_N] = tile_get(grid, vec2_init(tile->pos.x, tile->pos.y - 1)); 
	n.n[DIR_W] = tile_get(grid, vec2_init(tile->pos.x - 1, tile->pos.y));
	n.n[DIR_S] = tile_get(grid, vec2_init(tile->pos.x, tile->pos.y + 1));
	n.n[DIR_E] = tile_get(grid, vec2_init(tile->pos.x + 1, tile->pos.y));
	
	n.n[DIR_NW] = tile_get(grid, vec2_init(tile->pos.x - 1, tile->pos.y - 1));
	n.n[DIR_NE] = tile_get(grid, vec2_init(tile->pos.x + 1, tile->pos.y - 1));
	n.n[DIR_SW] = tile_get(grid, vec2_init(tile->pos.x - 1, tile->pos.y + 1));
	n.n[DIR_SE] = tile_get(grid, vec2_init(tile->pos.x + 1, tile->pos.y + 1));
	
	return n;

}

struct tile_unit_size
tile_hitbox_tiles(const struct tile_grid *grid, const struct hitbox *hb)
{		
	struct tile_unit_size tus;
	tus.tile_count = 0;

	int32_t left = 0;
	int32_t right = 0;
	int32_t top = 0;
	int32_t bottom = 0;

	hitbox_get_rect_size(hb, &left, &right, &top, &bottom);
	for (int32_t y = top; y <= bottom; y++)
	{
		for (int32_t x = left; x <= right; x++)
		{
			struct tile *t = tile_get(grid, vec2_init(x, y));
			tus.t[tus.tile_count] = t;
			tus.tile_count++;
		}	
	}		

	return tus;
}

vec2
tile_hitbox_unit_size(vec2 size)
{		
	/// THIS NEEDS TO USE grid->tile_size BEFORE IT WILL WORK CANT USE TILE_SIZE!!! NEED TO PASS IN GRID
//	if (size.x != 0) size.x = size.x / TILE_SIZE;
//	if (size.y != 0) size.y = size.y / TILE_SIZE;

	return size;
}

