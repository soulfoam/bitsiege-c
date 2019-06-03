#ifndef WORLD__
#define WORLD__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "../util/vec2.h"
#include "../util/s_list.h"
#include "../util/game_values.h"
#include "tile_grid.h"
#include "hero/hero.h"
#include "spell/spell.h"

struct
world
{
	uint16_t *id_pool;
	vec2 size;

	struct hero heroes[MAX_PLAYERS];
	struct spell spells[MAX_SPELLS];

	struct tile_grid grid;

	uint8_t hero_count;
	uint8_t spell_count;

};

struct world
world_init(void);

void
world_destroy(struct world *world);

void
world_update(struct world *world);

void
world_hero_init(struct world *world, struct hero *hero);

void
world_hero_remove(struct world *world, struct hero *hero);

void
world_keep_in_bounds(vec2 size, vec2 *pos);

struct hero*
world_hero_get(struct world *world, uint32_t id_game);

struct hero*
world_hero_get_at(struct world *world, vec2 pos);

struct hero*
world_hero_slot(struct world *world);

void
world_spell_init(struct world *world, struct spell *spell);

void
world_spell_remove(struct world *world, struct spell *spell);

struct spell*
world_spell_get(struct world *world, uint32_t id_game);

struct spell*
world_spell_slot(struct world *world);

#endif

