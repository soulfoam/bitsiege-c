#include "hero.h"
#include "../../data/data_info.h"
#include "../../util/log.h"
#include "../command.h"
#include "../world.h"

void
hero_init_base(
		struct hero *hero, 
		void *hero_data, 
		struct world *world,
		hero_updater update,
		uint16_t id_hero, 
		uint32_t id_game, 
		uint8_t team)
{
	const struct data_hero *data = data_hero_get(id_hero);
	hero->data = data;
	hero->hero_data = hero_data;
	hero->world = world;
	hero->update = update;

	hero->id_hero = id_hero;
	hero->id_game = id_game;
	vec2 pos = vec2_init(-10000, -10000);
	hero->pos = pos;
	hero->pos_next = pos;
	hero->input = 0;
	hero->action = HERO_ACTION_IDLE;
	hero->dir = DIR_S;
	hero->flags_hero = 0;
	hero->flags_world_snapshot = 0;
	hero->hp_base = data->hp;
	hero->hp = data->hp;
	hero->move_speed = data->move_speed;
	hero->cast_slot = 0;
	hero->cast_timer = 0;
	hero->cast_spell.id_game = USHRT_MAX;
	
	for (int i = 0; i < SPELL_SLOTS; i++) hero->cd_spells[i] = 0;
	
	hero->hitboxes[HERO_HITBOX_FULL] = data->hitbox_full;
	hero->hitboxes[HERO_HITBOX_BODY] = data->hitbox_body;
	hero->hitboxes[HERO_HITBOX_FEET] = data->hitbox_feet;

	hero->sight = 4;

	hero->team = team;
	hero->dead = false;
	hero->respawn_time = RESPAWN_TIME;
	hero->target = NULL;
	hero->networked = false;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		hero->heroes_in_range.heroes_seen[i] = NULL;
		hero->heroes_in_range.heroes_seen_prev[i] = NULL;
	}

	hero->ss = malloc(sizeof(struct hero));
	*hero->ss = *hero;

	log_trace("Hero initialized! %d %d", id_hero, id_game);

}

void
hero_init(struct hero *hero, struct world *world, uint16_t id_hero, uint32_t id_game, uint8_t team)
{	
	switch (id_hero)
	{
		case HERO_TYPE_ARCHER:
			hero_init_archer(hero, world, id_game, team);
		break;
	}
}

void
hero_init_username(struct hero *hero, char *username, union color color)
{
	strcpy(hero->username, username);
	hero->username_color = color;
}

void
hero_destroy(struct hero *hero)
{
	free(hero->ss);
	free(hero->hero_data);
}

void
hero_respawn(struct hero *hero, struct world *world)
{
	hero->respawn_time = RESPAWN_TIME;
	hero->dead = false;
	//pick spawn point
	vec2 pos = vec2_init(rand() % 300, rand() % 300);
	hero->pos = pos;
	hero->pos_next = pos;
}

void
hero_spell_cast(
		struct hero *caster, 
		uint8_t slot, 
		struct hero *target, 
		struct world *world)
{

	if (caster->cd_spells[slot] > 0) return;
	if (caster->cast_spell.id_game != USHRT_MAX) return;

	spell_init(&caster->cast_spell, caster, target, caster->data->spells[slot]->type, world);
	caster->cast_spell.id_game = 0;
	caster->cast_timer = caster->cast_spell.base_data->cast_time; //here we would reduce cast time from an item buff
	caster->cast_slot = slot;

}

static bool
hero_update_spell_cast(struct hero *hero, struct world *world)
{
	if (hero->cast_spell.id_game != USHRT_MAX)
	{
		bool cast_cancel = hero->input & FLAG_INPUT_CAST_CANCEL; 
		if (cast_cancel)
		{
			hero->cast_spell.id_game = USHRT_MAX;
			hero->cast_timer = 0;
			hero->action = HERO_ACTION_IDLE;
			return false;
		}

		if (hero->cast_timer > 0)
		{
			hero->cast_timer--;
		}	
		else
		{
			struct spell *s = world_spell_slot(world);
			struct spell *hs = &hero->cast_spell;
			spell_init(s, hs->caster, hs->target, hs->base_data->type, world);	
			world_spell_init(world, s);
			
			enum spell_type type = hs->base_data->type;
			
			float cooldown = hs->base_data->cooldown; //reduce cdr here from item buff

			if (type == 0) hero->cd_spells[SPELL_SLOT0] = cooldown;
			if (type == 1) hero->cd_spells[SPELL_SLOT1] = cooldown;
			if (type == 2) hero->cd_spells[SPELL_SLOT2] = cooldown;
			if (type == 3) hero->cd_spells[SPELL_SLOT3] = cooldown;
			if (type == 4) hero->cd_spells[SPELL_SLOT4] = cooldown;

			hero->cast_spell.id_game = USHRT_MAX;
			hero->cast_timer = 0;
			hero->action = HERO_ACTION_IDLE;
		}

	}
}

static void
hero_update_action(struct hero *hero, struct world *world)
{	
	
	//	log_trace("action: %d cast id %d", hero->action, hero->cast_spell.id_game);
	if (hero->cast_spell.id_game != USHRT_MAX)
	{		
		hero->action = hero_action_cast_from_slot(hero->cast_slot);
	//	log_trace("af action: %d cast id %d", hero->action, hero->cast_spell.id_game);
		return;
	}

	enum hero_action action = hero_action_from_input(hero->input);
	if (action == HERO_ACTION_WALK)
	{
		vec2 move = vec2_from_dir(move_dir_from_input(hero->input));
		move = vec2_mul(move, hero->move_speed);
		hero->pos = vec2_add(hero->pos, move);
		hero->pos_next = vec2_add(hero->pos, move);
	}

	hero->action = action;

}

static void
hero_update_core(struct hero *hero, struct world *world)
{
	
	for (int i = 0; i < HERO_HITBOX_COUNT; i++) hitbox_pos_set(&hero->hitboxes[i], hero->pos);
	
	for (int i = 0; i < SPELL_SLOTS; i++)
	{
		float *cd = &hero->cd_spells[i];
		if (*cd > 0) *cd -= 1;
		if (*cd <= 0) cd = 0;
	}

	if (hero->hp <= 0) hero->hp = 0;
	
	if (hero->dead) hero->respawn_time--;
	if (hero->respawn_time <= 0)
	{

	}

	struct heroes_in_range hir = tile_get_heroes_in_range(&world->grid, hero);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		hero->heroes_in_range.heroes_seen_prev[i] = hero->heroes_in_range.heroes_seen[i];
		hero->heroes_in_range.heroes_seen[i] = hir.heroes_seen[i];
	}

	world_keep_in_bounds(world->size, &hero->pos);
}

void
hero_update_network(struct hero *hero, struct world *world)
{
	hero_update_core(hero, world);
}

void
hero_update(struct hero *hero, struct world *world)
{		

	if (hero->networked) {hero_update_network(hero, world); return;}	
	
	hero_update_core(hero, world);
	hero_update_spell_cast(hero, world);	
	hero_update_action(hero, world);

}




