#ifndef SPELL__
#define SPELL__ 

#include "../../util/vec2.h"
#include "../../util/game_values.h"
#include "spell_info.h"
#include "../../data/data_spell.h"

struct spell;
struct world;
typedef void (*spell_updater)(struct spell *spell, struct world *world);

struct
spell
{
	uint16_t id_game;

	void *spell_data;
	const struct data_spell *base_data;
	struct world *world;

	struct hero *caster;
	struct hero *target;

	spell_updater update;
};

void
spell_init(
		struct spell *spell,
		struct hero *caster,
		struct hero *target,
		enum spell_type type, 
		struct world *world);


void
spell_update(struct spell *spell, struct world *world);

void
spell_destroy(struct spell *spell);

#endif


