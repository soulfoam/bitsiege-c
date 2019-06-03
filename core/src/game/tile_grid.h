#ifndef TILE_GRID__
#define TILE_GRID__

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../game/hitbox.h"
#include "../util/game_values.h"
#include "../game/hero/hero.h"

#define TILE_SIZE 260
#define GRID_SIZE 20
#define NEIGHBOR_AMOUNT 8

struct
tile_neighbors_list
{
	struct tile *n[8];
};

struct
tile_unit_size
{
	struct tile *t[MAX_UNIT_TILE_SIZE];
	uint8_t tile_count;
};

struct
tile
{
	uint32_t id; 
	vec2 pos;

	bool blocked;

	struct hero *heroes[MAX_PLAYERS];
	uint8_t hero_count;

	//pathfinding 
	int32_t h; //heur cost
	int32_t g; //move cost
	int32_t f; // h + g
	bool checked;
	struct tile *parent;
	struct tile *next;
	//////////
	
	uint16_t light; //used on client for fog of war
	
};

struct
tile_grid
{
	vec2 size;
	vec2 tile_size;
	struct tile *tiles;
};

struct tile_grid
tile_grid_init(vec2 grid_size, vec2 tile_size);

void
tile_grid_destroy(struct tile_grid *grid);

void
tile_reset(struct tile *tile);

void
tile_grid_reset(struct tile_grid *grid);

void
tile_grid_update(struct tile_grid *grid, struct hero *heroes);

void
tile_hero_add(struct tile *t, struct hero *hero);

void
tile_hero_remove(struct tile *t, struct hero *hero);

struct heroes_in_range
tile_get_heroes_in_range(struct tile_grid *grid, const struct hero *hero);

struct tile*
tile_get(const struct tile_grid *grid, vec2 pos);

struct tile*
tile_get_f(const struct tile_grid *grid, vec2 pos);

void
tile_set(struct tile *old_tile, const struct tile *new_tile);

void
tile_print(const struct tile *tile, const char *name);

struct tile_neighbors_list
tile_neighbors(const struct tile_grid *grid, const struct tile *tile);

struct tile_unit_size
tile_hitbox_tiles(const struct tile_grid *grid, const struct hitbox *hb);

vec2
tile_hitbox_unit_size(vec2 size);

#endif


