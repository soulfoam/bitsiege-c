#ifndef server_manager__
#define server_manager__

#include "network/client.h"
#include "game/world.h"
#include "network/server_funcs.h"
#include "util/s_list.h"
#include "util/id_pool.h"
#include "network/party.h"

struct
server_manager
{
	struct room *rooms[MAX_ROOMS];
	struct party *parties[MAX_PARTIES];
	struct party_invite *party_invites[MAX_USERS];
};

struct server_manager*
manager(void);

void
server_manager_init(void);

void
server_manager_destroy(void);

void
server_manager_update(void);

struct party*
server_manager_get_party(uint16_t id_party);

void
server_manager_que(struct client *cl);

#endif


