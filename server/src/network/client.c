#include <time.h>
#include "client.h"
#include "util/game_values.h"
#include "server.h"
#include "util/log.h"

struct client*
client_init(struct sockaddr_in addr_cl)
{
	struct client cl;
	cl.addr_cl = addr_cl;	

	cl.info.logged_in = false;

	cl.party = NULL;
	cl.pm = NULL;
	cl.room = NULL;
	cl.hero = NULL;

	cl.need_ping = false;
	cl.disconnect = false;

	cl.pack_buf = packet_buffer_init();

	log_trace("Client connected %s:%d", inet_ntoa(addr_cl.sin_addr), ntohs(addr_cl.sin_port));

	for (int i = 0; i < MAX_USERS; i++)
	{
		if (server()->clients[i].pack_buf == NULL)
		{
			server()->clients[i] = cl;
			server()->client_count++;
			return &server()->clients[i];
		}
	}

	return NULL;
}

void
client_remove(struct client *cl)
{

	if (cl->room != NULL)
	{
		room_member_remove(cl->room, cl->info.id_account);	
	//	// notify all room members
	}
	

	if (cl->party != NULL)
	{	
		party_member_remove(cl->party, cl->info.id_account);

		if (cl->party->member_count > 1)
		{
		
		}
		//notify party members
	}
	
	server()->client_count--;	

	free(cl->pack_buf);
	cl->pack_buf = NULL;

	cl->info.logged_in = false;

	cl->party = NULL;
	cl->pm = NULL;
	cl->room = NULL;
	cl->hero = NULL;

	cl->need_ping = false;
	cl->disconnect = false;


	log_trace("Client disconnected %s:%d", client_ip(cl), client_port(cl));

	struct sockaddr_in si = {};
	si.sin_port = ntohs(0);
	cl->addr_cl = si;

}

bool
sockaddr_in_match(
		struct sockaddr_in *addr, 
		struct sockaddr_in *addr2)
{
	if (addr->sin_addr.s_addr == addr2->sin_addr.s_addr 
			&& addr->sin_port == addr2->sin_port) 
		return true;

	return false;
}

struct client*
client_get(struct sockaddr_in *addr)
{
	for (int i = 0; i < MAX_USERS; i++)
	{
		struct client *cl = &server()->clients[i];
		if (sockaddr_in_match(&cl->addr_cl, addr))
		return cl;
	}

	return NULL;
}

bool
client_in_game(struct client *cl)
{
	if (cl->room == NULL || cl->room->game == NULL || cl->hero == NULL) return false;

	return true;
}

void
client_update(struct client *cl)
{	
	time_t now = time(NULL);
	
	if (cl->need_ping)
	{
		if (difftime(now, cl->last_time_packet) >= DISCONNECT_TIME)
		{
			cl->disconnect = true;
		}
	}	
	else
	{
		if (difftime(now, cl->last_time_packet) >= PING_TIME)
		{
			request_ping(cl);	
			cl->need_ping = true;
			cl->last_time_packet = time(NULL);
		}
	}

	if (cl->disconnect) client_remove(cl);
}

struct client*
client_get_id(uint32_t db_id)
{	
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (server()->clients[i].info.logged_in 
				&& server()->clients[i].info.id_account == db_id)
		{
			return &server()->clients[i];
		}
	}

	return NULL;
}

const char*
client_ip(const struct client *cl)
{
	return inet_ntoa(cl->addr_cl.sin_addr);
}

uint16_t
client_port(const struct client *cl)
{
	return ntohs(cl->addr_cl.sin_port);
}
