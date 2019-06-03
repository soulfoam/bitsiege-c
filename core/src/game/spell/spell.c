#include "spell.h"
#include "../../util/log.h"
#include "../../data/data_info.h"

static void
spell_init_data(struct spell *spell, struct hero *caster, struct hero *target, enum spell_type type)
{
	switch (type)
	{
		case SPELL_TYPE_ARCHER_ARROW:
			spell_init_archer_arrow(spell, caster);
		break;

		case SPELL_TYPE_ARCHER_TRAP:
			spell_init_archer_trap(spell, caster);
		break;
	}
}

void
spell_init(
		struct spell *spell,
		struct hero *caster,
		struct hero *target,
		enum spell_type type, 
		struct world *world)
{
	spell->world = world;
	spell->caster = caster;
	spell->target = target;

	spell_init_data(spell, caster, target, type);

}

void
spell_destroy(struct spell *spell)
{
	free(spell->spell_data);
}

void
spell_update(struct spell *spell, struct world *world)
{

}


