#ifndef HERO__ 
#define HERO__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "../../util/vec2.h"
#include "../direction.h"
#include "../../data/data_hero.h"
#include "../hitbox.h"
#include "hero_info.h"
#include "../../util/game_util.h"
#include "../spell/spell.h"

enum
hero_flags
{
	STUNNED = 1 << 0,
	SLOWED = 1 << 1,
	ROOTED = 1 << 2,
	SILENCED = 1 << 3,
	CONFUSED = 1 << 4
};

struct world;
struct hero;
typedef void(*hero_updater)(struct hero *hero, struct world *world);

#define HERO_HITBOX_FULL 0
#define HERO_HITBOX_BODY 1
#define HERO_HITBOX_FEET 2
#define HERO_HITBOX_COUNT (HERO_HITBOX_FULL + HERO_HITBOX_BODY + HERO_HITBOX_FEET)

struct
hero
{
	const struct data_hero *data;
	void *hero_data;

	struct world *world;

	uint32_t id_game;
	uint16_t id_hero;

	vec2 pos;
	vec2 pos_next;

	uint16_t input;	
	enum hero_action action;
	enum direction dir;	

	uint16_t flags_hero;

	float hp_base;
	float hp;
	float move_speed;
	uint16_t sight;
	uint16_t damage_magic;
	uint16_t damage_physical;
	uint8_t resist_magic;
	uint8_t resist_physical;
	
	uint8_t cast_slot;
	float cast_timer;
	struct spell cast_spell;

	float cd_spells[SPELL_SLOTS];
	
	struct hitbox hitboxes[HERO_HITBOX_COUNT];
	
	uint8_t team;

	bool dead;
	int8_t respawn_time;

	char username[MAX_USERNAME_LEN];
	union color username_color;
	
	struct heroes_in_range heroes_in_range;

	struct hero *ss; //snapshot of previous frame
	uint16_t flags_world_snapshot;

	struct hero *target;

	bool networked;

	hero_updater update;
};

void
hero_init_base(
		struct hero *hero, 
		void *hero_data, 
		struct world *world,
		hero_updater update,
		uint16_t id_hero, 
		uint32_t id_game, 
		uint8_t team);

void
hero_init(struct hero *hero, struct world *world, uint16_t id_hero, uint32_t id_game, uint8_t team);

void
hero_destroy(struct hero *hero);

void
hero_init_username(struct hero *hero, char *username, union color color);

void
hero_respawn(struct hero *hero, struct world *world);

void
hero_spell_cast(
		struct hero *caster, 
		uint8_t slot, 
		struct hero *target, 
		struct world *world);

void
hero_update(struct hero *hero, struct world *world);

#endif


