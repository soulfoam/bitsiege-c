#include <arpa/inet.h>
#include <time.h>
#include "server_packet.h"
#include "util/log.h"
#include "util/s_list.h"
#include "game/world.h"

static void
request_packet(
		struct client *cl, 
		char *buffer, 
		uint16_t len, 
		bool reliable, 
		enum packet_type type)
{
	packet_init(cl->pack_buf, buffer, len, reliable, type);
}

void
request_ping(struct client *cl)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	
	cl->last_time_packet = time(NULL);
	
	request_packet(cl, buffer, len, true, PACKET_PING);	
}

void
request_login_result(
		struct client *cl, 
		enum packet_result result, 
		struct friend_list *f_list)
{	
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	
	len += packet_write_int32(buffer + len, result);

	if (result == RESULT_USER_LOGIN_SUCCESS)
	{
		len += packet_write_uint32(buffer + len, cl->info.id_account);
		len += packet_write_uint8(buffer + len, f_list->friend_count);

		for (int i = 0; i < f_list->friend_count; i++)
		{
			struct client *c = client_get_id(f_list->friends[i].id);
			if (c != NULL) f_list->friends[i].online = true;

			len += packet_write_uint32(buffer + len, f_list->friends[i].id);
			len += packet_write_char(buffer + len, f_list->friends[i].username, MAX_USERNAME_LEN);
			len += packet_write_uint32(buffer + len, f_list->friends[i].name_color.c);
			len += packet_write_uint8(buffer + len, f_list->friends[i].online);
		}
	}
			
	request_packet(cl, buffer, len, true, PACKET_USER_LOGIN);	
}

void
request_register_result(struct client *cl, enum packet_result result)
{	

	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	len += packet_write_int32(buffer + len, result);

	request_packet(cl, buffer, len, true, PACKET_USER_REGISTER);	

}

void
request_party_invite(struct client *cl, struct client_info *inviter, uint16_t id_party)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
		
	len += packet_write_uint16(buffer + len, id_party);
	len += packet_write_char(buffer + len, inviter->username, MAX_USERNAME_LEN);
	len += packet_write_uint32(buffer + len, inviter->name_color.c);

	request_packet(cl, buffer, len, true, PACKET_PARTY_INVITE);	

}

void
request_party_join(struct client *cl, struct party *party)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	len += packet_write_uint8(buffer + len, party->member_count);

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		struct party_member *pm = &party->members[i];
		if (pm->id == UINT_MAX) continue;

		len += packet_write_uint32(buffer + len, pm->id);
		len += packet_write_char(buffer + len, pm->username, MAX_USERNAME_LEN);
		len += packet_write_uint32(buffer + len, pm->username_color.c);
	}

	request_packet(cl, buffer, len, true, PACKET_PARTY_JOIN);	

}

void
request_party_add(struct client *cl, struct party *party, uint32_t id)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		struct party_member *pm = &party->members[i];
		if (pm->id == UINT_MAX) continue;

		if (pm->id == id)
		{
			len += packet_write_uint32(buffer + len, pm->id);
			len += packet_write_char(buffer + len, pm->username, MAX_USERNAME_LEN);
			len += packet_write_uint32(buffer + len, pm->username_color.c);
		}
	}

	request_packet(cl, buffer, len, true, PACKET_PARTY_ADD);	
}

void
request_party_leave(struct client *cl, struct party *party, uint32_t id)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	for (int i = 0; i < MAX_PARTY_SIZE; i++)
	{
		struct party_member *pm = &party->members[i];
		if (pm->id == UINT_MAX) continue;

		if (pm->id == id)
		{
			len += packet_write_uint32(buffer + len, pm->id);
		}
	}

	request_packet(cl, buffer, len, true, PACKET_PARTY_LEAVE);	
}

void
request_room_join(struct client *cl, struct room *room)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	len += packet_write_uint8(buffer + len, room->member_count);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct client *cl = room->clients[i];
		if (cl == NULL) continue;

		len += packet_write_uint32(buffer + len, cl->pm->id);
		len += packet_write_char(buffer + len, cl->pm->username, MAX_USERNAME_LEN);
		len += packet_write_uint32(buffer + len, cl->pm->username_color.c);
		len += packet_write_uint8(buffer + len, cl->pm->team);
		len += packet_write_uint8(buffer + len, cl->pm->ready);
	}

	request_packet(cl, buffer, len, true, PACKET_ROOM_JOIN);	

}

void
request_room_add(struct client *cl, struct room *room, uint32_t id)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct client *cl = room->clients[i];
		if (cl == NULL) continue;

		if (cl->pm->id == id)
		{
			len += packet_write_uint32(buffer + len, cl->pm->id);
			len += packet_write_char(buffer + len, cl->pm->username, MAX_USERNAME_LEN);
			len += packet_write_uint32(buffer + len, cl->pm->username_color.c);
			len += packet_write_uint8(buffer + len, cl->pm->team);
		}
	}

	request_packet(cl, buffer, len, true, PACKET_ROOM_ADD);	
}

void
request_room_leave(struct client *cl, struct room *room, uint32_t id)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct client *cl = room->clients[i];
		if (cl == NULL) continue;

		if (cl->pm->id == id)
		{
			len += packet_write_uint32(buffer + len, id);
			break;
		}
	}

	request_packet(cl, buffer, len, true, PACKET_ROOM_LEAVE);	
}

void
request_room_game_start(struct client *cl, struct room *room)
{	
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	request_packet(cl, buffer, len, true, PACKET_ROOM_GAME_START);	

}

void
request_room_hero_change(struct client *cl, struct room *room)
{	
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	request_packet(cl, buffer, len, true, PACKET_ROOM_HERO_CHANGE);	
}

void
request_world_snapshot(struct client *cl, char *buffer, uint16_t len)
{	
	request_packet(cl, buffer, len, false, PACKET_WORLD_SNAPSHOT);	
}

void
request_input(struct client *cl)
{
//	struct entity_player *p = cl->info.cur_character;

	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

//	len += packet_write_uint32(buffer + len, (uint32_t)p->obj_info.pos.x);
//	len += packet_write_uint32(buffer + len, (uint32_t)p->obj_info.pos.y);


	request_packet(cl, buffer, len, false, PACKET_INPUT);	
}




