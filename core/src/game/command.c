#include "command.h"
#include "../network/packet.h"
#include "../util/log.h"

static void
command_init(
		struct command *command,
		enum command_type type, 
		void *data, 
		command_processer process)
{
	command->type = type;
	command->data = data;
	command->process = process;
	command->processed = false;
}

void
command_init_input(struct command *command, struct command_input *ci)
{	
	struct command_input *input = malloc(sizeof(struct command_world_snapshot));
	*input = *ci;

	command_init(command, COMMAND_INPUT, input, command_process_input);
}

void
command_init_world_snapshot(struct command *command, struct command_world_snapshot *ss)
{
	struct command_world_snapshot *snap = malloc(sizeof(struct command_world_snapshot));
	*snap = *ss;

	command_init(command, COMMAND_WORLD_SNAPSHOT, snap, command_process_world_snapshot);
}

void
command_init_hero_spell_cast(struct command *command, struct command_hero_spell_cast *sc)
{
	struct command_hero_spell_cast *cast = malloc(sizeof(struct command_hero_spell_cast));
	*cast = *sc;

	command_init(command, COMMAND_HERO_SPELL_CAST, cast, command_process_hero_spell_cast);
}

void
command_destroy(struct command *command)
{
	free(command->data);
}

void
command_process(struct command *command, struct world *world)
{
	command->process(command, world);
	command->processed = true;
}

void
command_process_input(struct command *command, struct world *world)
{
	struct command_input *data = command->data;	
	
	data->hero->input = data->input;
	data->hero->dir = data->dir;
}

void
command_process_world_snapshot(struct command *command, struct world *world)
{

	struct command_world_snapshot *ss = command->data;	

	for (int i = 0; i < ss->count_update_enemy; i++)
	{	
		const struct command_update_enemy *u = &ss->update_enemies[i];
		struct hero *h = world_hero_get(world, u->id);
		uint16_t flags_update = u->flags_update;
		h->flags_world_snapshot = flags_update;

		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_X)
		h->pos.x = u->pos.x;
		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_Y)
		h->pos.y = u->pos.y;
		if (flags_update & FLAG_WORLD_SNAPSHOT_INPUT)
		h->input = u->input;
		if (flags_update & FLAG_WORLD_SNAPSHOT_ACTION)
		h->action = u->action;
		if (flags_update & FLAG_WORLD_SNAPSHOT_DIR)
		h->dir = u->dir;
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP_BASE)
		h->hp_base = u->hp_base;
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP)
		h->hp = u->hp;
		if (flags_update & FLAG_WORLD_SNAPSHOT_FLAGS_HERO)
		h->flags_hero = u->flags_hero;

	}

	for (int i = 0; i < 1; i++)	
	{
		const struct command_update_local *u = &ss->update_local;

		struct hero *h = world_hero_get(world, u->id);
		uint16_t flags_update = u->flags_update;
		h->flags_world_snapshot = flags_update;

		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_X)
		h->pos.x = u->pos.x;
		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_Y)
		h->pos.y = u->pos.y;
		if (flags_update & FLAG_WORLD_SNAPSHOT_ACTION)
		h->action = u->action;
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP_BASE)
		h->hp_base = u->hp_base;
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP)
		h->hp = u->hp;
		if (flags_update & FLAG_WORLD_SNAPSHOT_FLAGS_HERO)
		h->flags_hero = u->flags_hero;
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD0)
		h->cd_spells[SPELL_SLOT0] = u->cd0;
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD1)
		h->cd_spells[SPELL_SLOT1] = u->cd1;
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD2)
		h->cd_spells[SPELL_SLOT2] = u->cd2;
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD3)
		h->cd_spells[SPELL_SLOT3] = u->cd3;
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD4)
		h->cd_spells[SPELL_SLOT4] = u->cd4;

	}

	for (int i = 0; i < ss->count_update_ally; i++)
	{	
		const struct command_update_ally *u = &ss->update_allies[i];
		struct hero *h = world_hero_get(world, u->id);
		uint16_t flags_update = u->flags_update;
		h->flags_world_snapshot = flags_update;

		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_X)
		h->pos.x = u->pos.x;
		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_Y)
		h->pos.y = u->pos.y;
		if (flags_update & FLAG_WORLD_SNAPSHOT_INPUT)
		h->input = u->input;
		if (flags_update & FLAG_WORLD_SNAPSHOT_ACTION)
		h->action = u->action;
		if (flags_update & FLAG_WORLD_SNAPSHOT_DIR)
		h->dir = u->dir;
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP_BASE)
		h->hp_base = u->hp_base;
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP)
		h->hp = u->hp;
		if (flags_update & FLAG_WORLD_SNAPSHOT_FLAGS_HERO)
		h->flags_hero = u->flags_hero;
		if (flags_update & FLAG_WORLD_SNAPSHOT_HAS_ULT)
		h->cd_spells[SPELL_SLOT4] = u->has_ult ? 0 : USHRT_MAX;
		
	}	

}

void
command_process_hero_spell_cast(struct command *command, struct world *world)
{
	struct command_hero_spell_cast *sc = command->data;	
	hero_spell_cast(sc->caster, sc->slot, sc->target, world);
}


