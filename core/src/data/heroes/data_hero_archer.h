#ifndef DATA_HERO_ARCHER__
#define DATA_HERO_ARCHER__

#include <inttypes.h>
#include "../../game/hero/hero.h"
#include "../../game/spell/spell.h"


#define SPELL_ARCHER_ARROW_COOLDOWN 10
#define SPELL_ARCHER_ARROW_CAST_TIME 0.1f
#define SPELL_ARCHER_ARROW_RANGE 120
#define SPELL_ARCHER_ARROW_DAMAGE 80
#define SPELL_ARCHER_ARROW_SPEED 4.2f
#define SPELL_ARCHER_ARROW_RANGE 120

#define SPELL_ARCHER_TRAP_DAMAGE 200
#define SPELL_ARCHER_TRAP_DURATION 10
#define SPELL_ARCHER_TRAP_STUN_TIME 2.5f
#define SPELL_ARCHER_TRAP_SIGHT 4

struct
spell_archer_arrow
{
	const float damage;
	const float speed;
	const float range;
};

struct
spell_archer_trap
{
	const float damage;
	const float duration;
	const float stun_time;
	const uint16_t sight;
};

void
hero_init_archer(struct hero *hero, struct world *world, uint32_t id_game, uint8_t team);

struct data_hero
data_hero_init_archer(void);

struct data_spell_set
data_spell_init_archer(void);

void
spell_init_archer_trap(struct spell *spell, struct hero *hero);

void
spell_init_archer_arrow(struct spell *spell, struct hero *hero);

#endif
