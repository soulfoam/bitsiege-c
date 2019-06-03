#ifndef ROOM__
#define ROOM__

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include "game/game.h"
#include "util/game_values.h"
#include "game/hero/hero.h"
#include "network/party.h"

enum
room_state
{
	ROOM_STATE_OPEN,
	ROOM_STATE_CLOSED
};

struct
room
{
	uint16_t id;
	struct client *clients[MAX_PLAYERS];

	uint8_t member_count;
	bool ranked;
	uint32_t elo;
	struct game *game;
	enum room_state status;
	
	uint16_t start_game_timer;	
};


struct room*
room_init(uint16_t id);

void
room_destroy(struct room *room);

void
room_update(struct room *room);

bool 
room_has_member(struct room *room, uint32_t id);

int8_t
room_can_join(struct room *room, uint8_t party_count);

void
room_member_add(struct room *room, struct client *cl);

void
room_member_remove(struct room *room, uint32_t id);

void
room_game_init(struct room *room);

#endif


