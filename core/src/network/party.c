#include "party.h"
#include "../util/log.h"

struct party_invite*
party_invite_init(uint16_t id_party, uint32_t id)
{
	struct party_invite *pi = malloc(sizeof(struct party_invite));
	pi->id_party = id_party;
	pi->id = id;

	return pi;
}

void
party_invite_destroy(struct party_invite *pi)
{
	free(pi);
}

struct party*
party_init(uint16_t id)
{
	struct party *party = malloc(sizeof(struct party));
	party->id = id;

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		party->members[i] = party_member_init(UINT_MAX, "NULL", COLOR_WHITE);
	}

	party->leader.id = -1;
	party->member_count = 0;

	return party;
}

void
party_destroy(struct party *party)
{
	free(party);
}

struct party_member
party_member_init(uint32_t id, char *username, uint32_t username_color)
{
	struct party_member pm;

	pm.id = id;
	strcpy(pm.username, username);
	pm.username_color.c = username_color;
	pm.team = TEAM0;
	pm.id_hero = 0;
	pm.ready = false;

	return pm;
}

struct party_member*
party_member_add(struct party *party, struct party_member pm)
{
	
	if (party->leader.id == UINT_MAX) party->leader = pm;

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		if (party->members[i].id == UINT_MAX)
		{
			party->members[i] = pm;
			party->member_count++;
			return &party->members[i];
		}
	}

	return NULL;
}

void
party_member_remove(struct party *party, uint32_t id)
{
	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		struct party_member *pm = &party->members[i];
		if (pm->id == UINT_MAX) continue;

		if (pm->id == id)
		{
			if (party->leader.id == id)
			{
				// make new leader code
			}
			party->members[i].id = -1;
			party->member_count--;
			break;
		}
	}
}

bool
party_has_member(struct party *party, uint32_t id)
{
	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		struct party_member *pm = &party->members[i];
		if (pm->id == UINT_MAX) continue;

		if (pm->id == id) return true;
	}

	return false;
}



