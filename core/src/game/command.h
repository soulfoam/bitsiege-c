#ifndef COMMAND__
#define COMMAND__

#include "../util/vec2.h"
#include "../util/game_values.h"
#include "hero/hero.h"
#include "world.h"

struct command;
typedef void (*command_processer)(struct command *command, struct world *world);

enum
command_type
{
	COMMAND_INVALID, 

	COMMAND_INPUT,
	COMMAND_WORLD_SNAPSHOT,
	COMMAND_HERO_SPELL_CAST,
};

struct
command_update_ally
{
	uint32_t id;

	uint16_t flags_update;
	vec2 pos;
	uint16_t input;
	uint8_t action;
	uint8_t dir;
	uint16_t hp_base;
	uint16_t hp;
	uint16_t flags_hero;
	uint8_t has_ult;
};

struct
command_update_enemy
{
	uint32_t id;

	uint16_t flags_update;
	vec2 pos;
	uint16_t input;
	uint8_t action;
	uint8_t dir;
	uint16_t hp_base;
	uint16_t hp;
	uint16_t flags_hero;
};

struct
command_update_local
{
	uint32_t id;

	uint16_t flags_update;
	vec2 pos;
	uint8_t action;
	uint16_t hp_base;
	uint16_t hp;
	uint16_t flags_hero;
	uint16_t cd0;
	uint16_t cd1;
	uint16_t cd2;
	uint16_t cd3;
	uint16_t cd4;
};

enum
world_snapshot_flags
{
	FLAG_WORLD_SNAPSHOT_POS_INTERP = 1 << 0,
	FLAG_WORLD_SNAPSHOT_POS_X = 1 << 1,
	FLAG_WORLD_SNAPSHOT_POS_Y = 1 << 2,
	FLAG_WORLD_SNAPSHOT_INPUT = 1 << 3,
	FLAG_WORLD_SNAPSHOT_ACTION = 1 << 4,
	FLAG_WORLD_SNAPSHOT_DIR = 1 << 5,
	FLAG_WORLD_SNAPSHOT_HP_BASE = 1 << 6,
	FLAG_WORLD_SNAPSHOT_HP = 1 << 7,
	FLAG_WORLD_SNAPSHOT_FLAGS_HERO = 1 << 8,
	FLAG_WORLD_SNAPSHOT_HAS_ULT = 1 << 9,
	FLAG_WORLD_SNAPSHOT_CD0 = 1 << 10,
	FLAG_WORLD_SNAPSHOT_CD1 = 1 << 11,
	FLAG_WORLD_SNAPSHOT_CD2 = 1 << 12,
	FLAG_WORLD_SNAPSHOT_CD3 = 1 << 13,
	FLAG_WORLD_SNAPSHOT_CD4 = 1 << 14
};

struct
command_world_snapshot
{
	uint8_t count_update_ally;
	uint8_t count_update_enemy;
	
	struct command_update_enemy update_enemies[MAX_PLAYERS];
	struct command_update_local update_local;
	struct command_update_ally update_allies[MAX_PLAYERS];
};

struct
command_input
{
	struct hero *hero;
	uint16_t input;
	enum direction dir;
};

struct
command_hero_spell_cast
{
	struct hero *caster;
	uint8_t slot;
	struct hero *target;
};

struct
command
{
	enum command_type type;
	void *data;

	command_processer process;

	bool processed;	
};

void
command_init_input(struct command *command, struct command_input *ci);

void
command_init_world_snapshot(struct command *command, struct command_world_snapshot *ss);

void
command_init_hero_spell_cast(struct command *command, struct command_hero_spell_cast *sc);

void
command_destroy(struct command *command);

void
command_process(struct command *command, struct world *world);

void
command_process_input(struct command *command, struct world *world);

void
command_process_world_snapshot(struct command *command, struct world *world);

void
command_process_hero_spell_cast(struct command *command, struct world *world);

#endif


