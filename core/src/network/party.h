#ifndef PARTY__
#define PARTY__

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "../util/game_util.h"
#include "../util/game_values.h"
#include "../game/hero/hero.h"

struct
party_invite
{
	uint16_t id_party;
	uint32_t id;
};

struct
party_member
{
	uint32_t id;
	char username[MAX_USERNAME_LEN];
	union color username_color;

	//in lobby stuff	
	uint16_t id_hero;
	uint8_t team;
	bool ready;
};

struct
party
{
	uint16_t id;
	struct party_member members[MAX_PARTY_SIZE]; 
	uint8_t member_count;
	struct party_member leader;
};

struct party_invite*
party_invite_init(uint16_t id_party, uint32_t id);

void
party_invite_destroy(struct party_invite *pi);

struct party*
party_init(uint16_t id);

void
party_destroy(struct party *party);

struct party_member
party_member_init(uint32_t id, char *name, uint32_t name_color);

struct party_member*
party_member_add(struct party *party, struct party_member pm);

void
party_member_remove(struct party *party, uint32_t id);

bool
party_has_member(struct party *party, uint32_t id);

#endif

