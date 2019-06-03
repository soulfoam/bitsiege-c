#include <ctype.h>
#include "packet_processor.h"
#include "game/world.h"
#include "util/util_funcs.h"
#include "util/log.h"
#include "util/id_pool.h"
#include "util/game_values.h"
#include "../database/db.h"
#include "server_funcs.h"
#include "util/s_list.h"
#include "network/friend.h"
#include "server.h"

static struct client *cl;

static void
process_disconnect(struct packet *packet, struct client *cl)
{
	cl->disconnect = true;
}

static void
process_user_register(struct packet *packet, struct client *cl)
{	
	char username[MAX_USERNAME_LEN+1];
	char password[MAX_PASSWORD_LEN+1];
	char email[MAX_EMAIL_LEN+1];

	uint16_t len = 0;
	
	len += packet_read_char(username, packet->buffer + len, MAX_USERNAME_LEN);
	len += packet_read_char(password, packet->buffer + len, MAX_PASSWORD_LEN);
	len += packet_read_char(email, packet->buffer + len, MAX_EMAIL_LEN);

	enum packet_result result = db_account_register(
			username, 
			password, 
			email, 
			cl->addr_cl.sin_addr.s_addr);	

	request_register_result(cl, result);
}

static void
process_user_login(struct packet *packet, struct client *cl)
{
	if (cl->info.logged_in) 
	{
		log_warn(
				"Client: %s:%d tried logging in but is already logged in.", 
				client_ip(cl), client_port(cl));
		return;

	}
	char username[MAX_USERNAME_LEN+1];
	char password[MAX_PASSWORD_LEN+1];

	uint16_t len = 0;
	
	len += packet_read_char(username, packet->buffer + len, MAX_USERNAME_LEN);
	len += packet_read_char(password, packet->buffer + len, MAX_PASSWORD_LEN);

	int64_t id_account = db_get_account_id(username);

	enum packet_result result; 

	if (account_is_logged_in(id_account))
	{
		result = RESULT_USER_LOGIN_ACCOUNT_LOGGED_IN;	
		log_warn(
				"Client %s:%d tried logging into acc %lu that is already logged in.", 
				client_ip(cl),
				client_port(cl),
				id_account);

		request_login_result(cl, result, NULL);
		return;
	}
	else
	{
		result = db_account_login(cl, username, password);
	}

	if (result == RESULT_USER_LOGIN_SUCCESS)
	{
		cl->party = party_init(id_pool_get(id_pool_party(), MAX_PARTIES));

		struct party_member pm = party_member_init(
				cl->info.id_account, 
				cl->info.username, 
				cl->info.name_color.c);

		cl->pm = party_member_add(cl->party, pm);

		for (int i = 0; i < MAX_PARTIES; i++)
		{
			if (manager()->parties[i] == NULL) 
			{
				manager()->parties[i] = cl->party;
				break;
			}
		}

		cl->info.logged_in = true;
	}

	int8_t friend_result;

	struct friend_list f_list = db_account_friend_list_get(id_account, &friend_result);
	for (int i = 0; i < MAX_FRIENDS; i++)
	{
		struct friend *f = &f_list.friends[i];
		if (f->id == UINT_MAX) continue;

		if (client_get_id(f->id) != NULL) f->online = true;
	}

	struct friend_list *f_p = NULL;
	
	if (friend_result == 1) f_p = &f_list;
		
	request_login_result(cl, result, f_p);
}

static void
process_room_join(struct packet *packet, struct client *cl)
{	

	uint16_t len = 0;

	//uint32_t db_id; 
	//len += packet_read_uint32(&db_id, packet->buffer + len);
	
	server_manager_que(cl);	

}

static void
process_party_invite(struct packet *packet, struct client *cl)
{

	uint16_t len = 0;	

	uint32_t id = UINT_MAX;
	len += packet_read_uint32(&id, packet->buffer + len);	

	struct client *c = client_get_id(id);
	
	if (c == NULL)
	{
		log_warn(
				"Client: %s:%d tried sending a party invite to someone who is offline.", 
				client_ip(cl), client_port(cl));
	}

	struct party_invite *pi = party_invite_init(cl->party->id, id);
	
	for (int i = 0; i < MAX_USERS; i++)
	{
		if (manager()->party_invites[i] == NULL) manager()->party_invites[i] = pi;
	}
		
	request_party_invite(c, &cl->info, cl->party->id);
}

static void
process_party_invite_accept(struct packet *packet, struct client *cl)
{
	uint16_t len = 0;
	
		
	uint16_t id_party = 0;
	len += packet_read_uint16(&id_party, packet->buffer + len);	

	for (int i = 0; i < MAX_USERS; i++)
	{
		struct party_invite *pi = manager()->party_invites[i];
		if (pi == NULL) continue;

		if (pi->id_party == id_party && pi->id == cl->info.id_account)
		{
			cl->party = server_manager_get_party(id_party);

			struct party_member pm = party_member_init(
					cl->info.id_account, 
					cl->info.username, 
					cl->info.name_color.c);

			pm.team = TEAM0;

			cl->pm = party_member_add(cl->party, pm);
			party_invite_destroy(pi);
			manager()->party_invites[i] = NULL;

			break;
		}
	}
	
	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		if (cl->party->members[i].id == UINT_MAX) continue;

		struct client *c = client_get_id(cl->party->members[i].id);
		request_party_join(c, cl->party);
	}		
}

static void
process_party_invite_decline(struct packet *packet, struct client *cl)
{
	uint16_t len = 0;
	

	uint16_t id_party = 0;
	len += packet_read_uint16(&id_party, packet->buffer + len);	

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		struct party_invite *pi = manager()->party_invites[i];
		if (pi == NULL) continue;

		if (pi->id_party == id_party && pi->id == cl->info.id_account)
		{
			party_invite_destroy(manager()->party_invites[i]);
			manager()->party_invites[i] = NULL;
			break;
		}
	}
}

static void
process_party_leave(struct packet *packet, struct client *cl)
{
	cl->party = party_init(id_pool_get(id_pool_party(), MAX_PARTIES));

	struct party_member pm = party_member_init(
			cl->info.id_account, 
			cl->info.username, 
			cl->info.name_color.c);

	party_member_add(cl->party, pm);

	for (int i = 0; i < MAX_PARTIES; i++)
	{
		if (manager()->parties[i] == NULL) manager()->parties[i] = cl->party;
	}
	
	request_party_join(cl, cl->party);
}

static void
process_party_que(struct packet *packet, struct client *cl)
{

	server_manager_que(cl);
}

static void
process_input(struct packet *packet, struct client *cl)
{

	if (!client_in_game(cl)) return;

	uint16_t len = 0;
	uint16_t input = 0;
	uint8_t dir = 0;
	
	len += packet_read_uint16(&input, packet->buffer + len);	
	len += packet_read_uint8(&dir, packet->buffer + len);	

	if (cl->hero->input == input && cl->hero->dir == dir) return; 
	
	struct command_input ci = 
	{
		cl->hero,
		input,
		dir
	};
	
	command_init_input(game_command_init(cl->room->game), &ci);
		
}

static void
process_spell_cast(struct packet *packet, struct client *cl)
{

	if (!client_in_game(cl)) return;

	uint16_t len = 0;

	uint8_t slot = 0;
	uint32_t id_target = 0;
	
	len += packet_read_uint8(&slot, packet->buffer + len);	
	len += packet_read_uint32(&id_target, packet->buffer + len);	
	
	if (slot > SPELL_SLOTS) return;

	struct command_hero_spell_cast hsc = 
	{
		cl->hero,
		slot,
		world_hero_get(&cl->room->game->world, id_target)
	};
	
	log_trace("Processing spell cast: slot %u target id %u", slot, id_target);

	command_init_hero_spell_cast(game_command_init(cl->room->game), &hsc);
		
}

void
process_ping(struct packet *packet, struct client *cl)
{

}

void
process_packet(struct packet *packet, struct client *cl)
{
	
	if (packet == NULL)	return;
	
	if (!cl->info.logged_in)
	{
		if (packet->type != PACKET_USER_REGISTER 
				&& packet->type != PACKET_USER_LOGIN
				&& packet->type != PACKET_PING
				&& packet->type != PACKET_ACK)
		{
			log_warn(
					"Client: %s:%d tried sending packet of type %d but isn't logged in...", 
					client_ip(cl), client_port(cl), packet->type);
			return;
		}
	}

	switch (packet->type)
	{
		case PACKET_DISCONNECT:
			process_disconnect(packet, cl);
		break;

		case PACKET_USER_REGISTER:
			process_user_register(packet, cl);
		break;

		case PACKET_USER_LOGIN:
			process_user_login(packet, cl);
		break;

		case PACKET_ROOM_JOIN: 
			process_room_join(packet, cl);
		break;
	
		case PACKET_PARTY_INVITE:
			process_party_invite(packet, cl);
		break;

		case PACKET_PARTY_INVITE_ACCEPT:
			process_party_invite_accept(packet, cl);
		break;

		case PACKET_PARTY_INVITE_DECLINE:
			process_party_invite_decline(packet, cl);
		break;

		case PACKET_PARTY_QUE:
			process_party_que(packet, cl);
		break;

		case PACKET_INPUT:
			process_input(packet, cl);
		break;

		case PACKET_WORLD_SPELL_CAST:
			process_spell_cast(packet, cl);
		break;

		case PACKET_PING:
			process_ping(packet, cl);
		break;

		default: break;
	}

}


