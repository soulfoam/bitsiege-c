#include <time.h>
#include "world.h"
#include "../util/log.h"
#include "../util/id_pool.h"
#include "../util/util_funcs.h"

struct world
world_init(void)
{
	srand(44934);

	struct world world;

	world.id_pool = id_pool_init(MAX_ENTITIES);
	world.grid = tile_grid_init(vec2_init(GRID_SIZE, GRID_SIZE), vec2_init(TILE_SIZE, TILE_SIZE));

	world.size = vec2_init(1000, 1000);

	for (int i = 0; i < MAX_PLAYERS; i++) world.heroes[i].id_game = UINT_MAX;
	for (int i = 0; i < MAX_SPELLS; i++) world.spells[i].id_game = USHRT_MAX;


	log_info("World initialized.");

	return world;
}

void
world_destroy(struct world *world)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct hero *hero = &world->heroes[i];
		if (hero->id_game == UINT_MAX) continue;
		
		hero_destroy(hero);
	}

	for (int i = 0; i < MAX_SPELLS; i++)
	{
		struct spell *spell = &world->spells[i];
		if (spell->id_game == USHRT_MAX) continue;
		
		spell_destroy(spell);
	}

	id_pool_destroy(world->id_pool);
	tile_grid_destroy(&world->grid);
}

void
world_keep_in_bounds(vec2 size, vec2 *pos)
{
	if (pos->x <= 0) pos->x = 0;
	if (pos->y <= 0) pos->y = 0;
	if (pos->x >= size.x) pos->x = size.x;
	if (pos->y >= size.y) pos->y = size.y;
}

void
world_update(struct world *world)
{

	tile_grid_update(&world->grid, world->heroes);	

	for (int i = 0; i < MAX_SPELLS; i++)
	{	
		struct spell *spell = &world->spells[i];
		if (spell->id_game == USHRT_MAX) continue;
		
		spell->update(spell, world);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct hero *hero = &world->heroes[i];
		if (hero->id_game == UINT_MAX) continue;
		
		hero->update(hero, world);
	}

}

void
world_hero_init(struct world *world, struct hero *hero) 
{
	hero_respawn(hero, world);	
	world->hero_count++;
}

void
world_hero_remove(struct world *world, struct hero *hero)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (world->heroes[i].id_game == UINT_MAX) continue;

		if (world->heroes[i].id_game == hero->id_game)
		{
			world->heroes[i].id_game = UINT_MAX;
			break;
		}
	}

	world->hero_count--;
}

struct hero*
world_hero_get(struct world *world, uint32_t id_game)
{	
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct hero *hero = &world->heroes[i];
		if (hero->id_game == UINT_MAX) continue;

		if (hero->id_game == id_game) return hero;
	}

	return NULL;
}

struct hero*
world_hero_get_at(struct world *world, vec2 pos)
{
	struct hitbox hb = hitbox_init(vec2_init(0, 0), vec2_init(1, 1));
	hb.pos = pos;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct hero *h = &world->heroes[i];
		if (h->id_game == UINT_MAX) continue;

		if (hitbox_collides(&hb, &h->hitboxes[HERO_HITBOX_FULL])) return h;
	}

	return NULL;
}

struct hero*
world_hero_slot(struct world *world)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (world->heroes[i].id_game == UINT_MAX)
		{
			return &world->heroes[i];
		}
	}

	return NULL;
}

void
world_spell_init(struct world *world, struct spell *spell)
{
	spell->id_game = id_pool_get(world->id_pool, MAX_ENTITIES);
	world->spell_count++;
}

void
world_spell_remove(struct world *world, struct spell *spell)
{
	for (int i = 0; i < MAX_SPELLS; i++)
	{
		struct spell *s = &world->spells[i];
		if (s->id_game == USHRT_MAX) continue;

		if (s->id_game == spell->id_game)
		{
			id_pool_put(world->id_pool, s->id_game, MAX_ENTITIES);
			s->id_game = USHRT_MAX;
			spell_destroy(s);
			break;
		}
	}

	world->spell_count--;
}

struct spell*
world_spell_get(struct world *world, uint32_t id_game)
{	
	for (int i = 0; i < MAX_SPELLS; i++)
	{
		struct spell *spell = &world->spells[i];
		if (spell->id_game == USHRT_MAX) continue;

		if (spell->id_game == id_game) return spell;
	}

	return NULL;
}

struct spell*
world_spell_slot(struct world *world)
{
	for (int i = 0; i < MAX_SPELLS; i++)
	{
		if (world->spells[i].id_game == USHRT_MAX)
		{
			return &world->spells[i];
		}
	}

	return NULL;
}


