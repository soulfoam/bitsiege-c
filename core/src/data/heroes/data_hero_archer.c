#include <stdio.h>
#include <stdlib.h>
#include "data_hero_archer.h"
#include "../data_spell.h"
#include "../data_hero.h"

static void
hero_update_archer(struct hero *hero, struct world *world)
{	
	hero_update(hero, world);
}

void
hero_init_archer(struct hero *hero, struct world *world, uint32_t id_game, uint8_t team)
{
	struct hero_archer *archer = malloc(sizeof(struct hero_archer));
	archer->fire_arrow_count = 0;
	hero_init_base(hero, archer, world, hero_update_archer, HERO_TYPE_ARCHER, id_game, team);
}

struct data_hero
data_hero_init_archer(void)
{
	struct data_hero data;

	data.type = HERO_TYPE_ARCHER;
	strcpy(data.name, "Archer");
	

	data.hitbox_full = hitbox_init(vec2_init(0, 0), vec2_init(9, 21));
	data.hitbox_body = hitbox_init(vec2_init(0, 5), vec2_init(9, 12));
	data.hitbox_feet = hitbox_init(vec2_init(0, 19), vec2_init(9, 2));

	data.hp = 2500;
	data.move_speed = 3.75f;
	data.sight = 4;

	data.damage_physical = 100;
	data.damage_magic = 10;
	data.resist_physical = 0;
	data.resist_magic = 0;

	const struct data_spell_set *spell_set = data_spell_set_get(data.type);

	for (int i = 0; i < SPELL_SLOTS; i++) data.spells[i] = &spell_set->spells[i];


	return data;
}

static struct data_spell
data_spell_init_arrow(void)
{
	struct data_spell arrow;
	arrow.type = SPELL_TYPE_ARCHER_ARROW;
	arrow.cooldown = 0.5f;
	arrow.cast_time = 1.0f;
	arrow.channeled = false;
	arrow.hitbox = hitbox_init(vec2_init(0, 0), vec2_init(9, 3));
	arrow.remove_on_death = false;	
	
	return arrow;
}

static struct data_spell
data_spell_init_trap(void)
{
	struct data_spell trap;
	trap.type = SPELL_TYPE_ARCHER_TRAP;
	trap.cooldown = 10;

	trap.cast_time = 0.1f;
	trap.channeled = false;
	trap.hitbox = hitbox_init(vec2_init(0, 0), vec2_init(9, 3));
	trap.remove_on_death = false;	
	
	return trap;
}

struct data_spell_set
data_spell_init_archer(void)
{
	struct data_spell_set set;
	set.hero_type = HERO_TYPE_ARCHER;

	set.spells[SPELL_SLOT0] = data_spell_init_arrow();
	set.spells[SPELL_SLOT1] = data_spell_init_trap();

	return set;
}

static void
spell_update_archer_arrow(struct spell *spell, struct world *world)
{
	struct spell_archer_arrow *arrow = spell->spell_data;
	
}

void
spell_init_archer_arrow(struct spell *spell, struct hero *hero)
{
	struct spell_archer_arrow arrow = {
		SPELL_ARCHER_ARROW_DAMAGE, 
		SPELL_ARCHER_ARROW_SPEED,
		SPELL_ARCHER_ARROW_RANGE};

	struct spell_archer_arrow *arrow_p = malloc(sizeof(struct spell_archer_arrow));
	memcpy(arrow_p, &arrow, sizeof(struct spell_archer_arrow));
	
	spell->update = spell_update_archer_arrow;
	spell->base_data = hero->data->spells[SPELL_TYPE_ARCHER_ARROW];
	spell->spell_data = arrow_p;

	
}

static void
spell_update_archer_trap(struct spell *spell, struct world *world)
{
	struct spell_archer_trap *trap = spell->spell_data;
	
}

void
spell_init_archer_trap(struct spell *spell, struct hero *hero)
{
	struct spell_archer_trap trap = {
		SPELL_ARCHER_TRAP_DAMAGE,
		SPELL_ARCHER_TRAP_DURATION,
		SPELL_ARCHER_TRAP_STUN_TIME,
		SPELL_ARCHER_TRAP_SIGHT
	};

	struct spell_archer_arrow *trap_p = malloc(sizeof(struct spell_archer_trap));
	memcpy(trap_p, &trap, sizeof(struct spell_archer_trap));
	
	spell->update = spell_update_archer_trap;
	spell->base_data = hero->data->spells[SPELL_TYPE_ARCHER_TRAP];
	spell->spell_data = trap_p;

	
}

