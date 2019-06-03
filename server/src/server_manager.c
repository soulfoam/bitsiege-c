#include "server_manager.h"
#include "network/server.h"
#include "room/room.h"
#include "network/server_funcs.h"
#include "network/server_packet.h"
#include "util/log.h"
#include "network/packet.h"

static struct server_manager mngr;

struct server_manager*
manager(void)
{
	return &mngr;
}

void
server_manager_init(void)
{	
	for (int i = 0; i < MAX_ROOMS; i++)
	{
		mngr.rooms[i] = NULL;
	}

	for (int i = 0; i < MAX_PARTIES; i++)
	{
		mngr.parties[i] = NULL;
	}

	for (int i = 0; i < MAX_USERS; i++)
	{
		mngr.party_invites[i] = NULL;
	}
}

void
server_manager_destroy(void)
{
	for (int i = 0; i < MAX_ROOMS; i++)
	{
		room_destroy(mngr.rooms[i]);
	}

	for (int i = 0; i < MAX_PARTIES; i++)
	{
		party_destroy(mngr.parties[i]);
	}

	for (int i = 0; i < MAX_PARTIES; i++)
	{
		party_invite_destroy(mngr.party_invites[i]);
	}


	log_trace("Server manager freed.");
}

void
server_manager_update(void)
{
	for (int i = 0; i < MAX_ROOMS; i++)
	{
		struct room *room = mngr.rooms[i];
		if (room == NULL) continue;

		if (room->member_count == 0)
		{
			id_pool_put(id_pool_room(), room->id, MAX_USERS);
			room_destroy(room);
			mngr.rooms[i] = NULL;
			continue;
		}

		room_update(room);

	}

	for (int i = 0; i < MAX_PARTIES; i++)
	{
		struct party *p = mngr.parties[i];
		if (p == NULL) continue;

		if (p->member_count == 0)
		{
			id_pool_put(id_pool_party(), p->id, MAX_PARTIES);
			party_destroy(p);
			mngr.parties[i] = NULL;
			continue;
		}
	}

}

uint16_t
server_manager_room_count(void)
{
	uint16_t count = 0;

	for (int i = 0; i < MAX_ROOMS; i++)
	{
		if (mngr.rooms[i] == NULL) continue;

		count++;
	}

	return count;
}

static void
server_manager_room_init(struct client *cl)
{
	struct room *r = room_init(id_pool_get(id_pool_room(), MAX_ROOMS));

	for (int i = 0; i < MAX_ROOMS; i++)
	{
		if (mngr.rooms[i] == NULL)
		{
			mngr.rooms[i] = r;
			break;
		}
	}

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		if (cl->party->members[i].id == UINT_MAX) continue;

		struct client *c = client_get_id(cl->party->members[i].id);
		c->room = r;
		cl->party->members[i].team = TEAM0;

		room_member_add(r, c);
	}

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		if (cl->party->members[i].id == UINT_MAX) continue;

		request_room_join(cl, r);
	}
}

void
server_manager_que(struct client *cl)
{
	if (cl->party->leader.id == cl->info.id_account)
	{	
		for (int i = 0; i < MAX_PARTY_SIZE; i++)
		{
			struct party_member *pm = &cl->party->members[i];
			if (pm->id == UINT_MAX) continue;

			struct client *cpm = client_get_id(pm->id);
			if (cpm->room != NULL) return; // someone is already in a game somehow
		}

		if (server_manager_room_count() == 0)
		{
			server_manager_room_init(cl);
			return;
		}
	
		for (int i = 0; i < MAX_ROOMS; i++)
		{
			struct room *room = mngr.rooms[i];
			if (room == NULL) continue;

			int8_t join = room_can_join(room, cl->party->member_count);

			if (join != -1)
			{
				for (int i = 0; i < MAX_PARTY_SIZE; i++)
				{
					struct party_member *pm = &cl->party->members[i];
					if (pm->id == UINT_MAX) continue;

					pm->team = join;

					struct client *c = client_get_id(pm->id);
					c->room = room;

					room_member_add(room, c);

					request_room_join(c, room);

					log_trace("Joining previous room on team: %d", join);
					// maybe throw this in its own procedure?
					//
				}
					
				return;
			}
			else
			{
				server_manager_room_init(cl);
				return;
			}
		}
	}

}

struct party*
server_manager_get_party(uint16_t id_party)
{	
	for (int i = 0; i < MAX_PARTIES; i++)
	{
		if (mngr.parties[i]->id == id_party) return mngr.parties[i];
	}

	return NULL;
}

struct party*
server_manager_get_open_party(uint16_t id_party)
{	

}



