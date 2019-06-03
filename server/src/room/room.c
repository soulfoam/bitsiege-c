#include "room.h"
#include "game/world.h"
#include "util/log.h"
#include "network/party.h"
#include "../network/client.h"
#include "../network/server_packet.h"

struct room*
room_init(uint16_t id)
{
	struct room *room = malloc(sizeof(struct room));
	
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		//room->members[i] = party_member_init(-1, "NULL", COLOR_WHITE);
		room->clients[i] = NULL;
	}

	room->id = id;
	room->member_count = 0;
	room->ranked = false;
	room->elo = 0;
	room->game = NULL;
	room->start_game_timer = ROOM_START_GAME_TIME;
	room->status = ROOM_STATE_OPEN;

	log_trace("Room created %d.", id);

	return room;
}

void
room_destroy(struct room *room)
{
	if (room->status == ROOM_STATE_CLOSED)
	{
		game_destroy(room->game);
	}	

	log_trace("Room freed %d.", room->id);

	free(room);

}

static void
room_snapshot_enemies(
		struct room *room, 
		struct hero *h, 
		char *buffer, 
		uint16_t *len,
		bool full_update)
{

	struct hero *seen = NULL;
	struct hero *seen_prev = NULL;
	
	uint8_t update_count = 0;
	for (int s = 0; s < MAX_PLAYERS; s++)
	{
		seen = h->heroes_in_range.heroes_seen[s];
		if (seen == NULL) continue;
		if (h->team == seen->team) continue;
		
		for (int sp = 0; sp < MAX_PLAYERS; sp++)
		{	
			seen_prev = h->heroes_in_range.heroes_seen_prev[sp];
			if (seen_prev == NULL) continue;

			if (seen_prev->id_game == seen->id_game)
			{
				seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_INTERP;

				*len += packet_write_uint32(buffer + *len, seen->id_game);
				uint16_t len_f = *len;
				*len += sizeof(seen->flags_world_snapshot);

				if (seen->pos.x != seen->ss->pos.x || full_update) 
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_X; 
					*len += packet_write_float(buffer + *len, seen->pos.x);
				}

				if (seen->pos.y != seen->ss->pos.y || full_update) 
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_Y; 
					*len += packet_write_float(buffer + *len, seen->pos.y);
				}

				if (seen->input != seen->ss->input || full_update)
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_INPUT; 
					*len += packet_write_uint16(buffer + *len, seen->input);
				}

				if (seen->action != seen->ss->action || full_update)
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_ACTION; 
					uint8_t action = (uint8_t) seen->action;
					*len += packet_write_uint8(buffer + *len, action);
				}

				if (seen->dir != seen->ss->dir || full_update)
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_DIR; 
					uint8_t dir = (uint8_t) seen->dir;
					*len += packet_write_uint8(buffer + *len, dir);
				}

				if (seen->hp_base != seen->ss->hp_base || full_update)
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_HP_BASE;
					*len += packet_write_uint16(buffer + *len, seen->hp_base);
				}

				if (seen->hp != seen->ss->hp || full_update)
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_HP;
					*len += packet_write_uint16(buffer + *len, seen->hp);
				}

				if (seen->flags_hero != seen->ss->flags_hero || full_update)
				{
					seen->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_FLAGS_HERO;
					*len += packet_write_uint16(buffer + *len, seen->flags_hero);
				}

				packet_write_uint16(buffer + len_f, seen->flags_world_snapshot);

				update_count++;
			}
		
		}
		
	}

	packet_write_uint8(buffer + 1, update_count);


//	uint8_t oor_count = 0;
//	for (int sp = 0; sp < MAX_PLAYERS; sp++)
//	{	
//		seen_prev = h->heroes_in_range.heroes_seen_prev[sp];
//		if (seen_prev == NULL) continue;
//		
//		bool contains = false;
//		for (int s = 0; s < MAX_PLAYERS; s++)
//		{
//			seen = h->heroes_in_range.heroes_seen[s];
//			if (seen == NULL) continue;
//			
//			if (seen_prev->id_game == seen->id_game) contains = true;
//		}
//		
//		if (!contains && !seen_prev->team)
//		{
//			*len += packet_write_uint32(buffer + *len, h->id_game);
//			oor_count++;
//		}
//
//	}
//
//	packet_write_uint8(buffer + 2, oor_count);
	

}

static void
room_snapshot_allies(
		struct room *room, 
		struct hero *h, 
		char *buffer, 
		uint16_t *len,
		bool full_update)
{
	struct hero *o = NULL;
	uint8_t ally_count = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		o = &room->game->world.heroes[i];			

		if (o->id_game == UINT_MAX) continue;

		if (h->team == o->team && h->id_game != o->id_game)
		{
			o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_INTERP;

			*len += packet_write_uint32(buffer + *len, o->id_game);
			uint16_t len_f = *len;
			*len += sizeof(h->flags_world_snapshot);

			if (o->pos.x != o->ss->pos.x || full_update) 
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_X; 
				*len += packet_write_float(buffer + *len, o->pos.x);
			}

			if (o->pos.y != o->ss->pos.y || full_update) 
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_Y; 
				*len += packet_write_float(buffer + *len, o->pos.y);
			}

			if (o->input != o->ss->input || full_update)
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_INPUT; 
				*len += packet_write_uint16(buffer + *len, o->input);
			}

			if (o->action != o->ss->action || full_update)
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_ACTION; 
				uint8_t action = (uint8_t) o->action;
				*len += packet_write_uint8(buffer + *len, action);
			}

			if (o->dir != o->ss->dir || full_update)
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_DIR; 
				uint8_t dir = (uint8_t) o->dir;
				*len += packet_write_uint8(buffer + *len, dir);
			}

			if (o->hp_base != o->ss->hp_base || full_update)
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_HP_BASE;
				*len += packet_write_uint16(buffer + *len, o->hp_base);
			}

			if (o->hp != o->ss->hp || full_update)
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_HP;
				*len += packet_write_uint16(buffer + *len, o->hp);
			}

			if (o->flags_hero != o->ss->flags_hero || full_update)
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_FLAGS_HERO;
				*len += packet_write_uint16(buffer + *len, o->flags_hero);
			}

			uint8_t has_ult = (o->cd_spells[SPELL_SLOT4] <= 0);
			uint8_t has_ult_prev = (o->ss->cd_spells[SPELL_SLOT4] <= 0);

			if (has_ult != has_ult_prev || full_update)
			{
				o->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_HAS_ULT;
				*len += packet_write_uint8(buffer + *len, has_ult);
			}

			packet_write_uint16(buffer + len_f, o->flags_world_snapshot);

			ally_count++;
		}
	}

	packet_write_uint8(buffer, ally_count);
}

static void
room_snapshot_local(
		struct room *room, 
		struct hero *h, 
		char *buffer, 
		uint16_t *len, 
		bool full_update)
{
	h->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_INTERP;
	
	*len += packet_write_uint32(buffer + *len, h->id_game);
	uint16_t len_f = *len;
	*len += sizeof(h->flags_world_snapshot);

	if (h->pos.x != h->ss->pos.x || full_update)
	{
		h->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_X; 
		*len += packet_write_float(buffer + *len, h->pos.x);
	}

	if (h->pos.y != h->ss->pos.y || full_update)
	{
		h->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_POS_Y; 
		*len += packet_write_float(buffer + *len, h->pos.y);

	}
	if (h->action != h->ss->action || full_update)
	{
		h->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_ACTION; 
		uint8_t action = (uint8_t) h->action;
		*len += packet_write_uint8(buffer + *len, action);
	}

	if (h->hp_base != h->ss->hp_base || full_update)
	{
		h->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_HP_BASE;
		*len += packet_write_uint16(buffer + *len, h->hp_base);
	}

	if (h->hp != h->ss->hp || full_update)
	{
		h->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_HP;
		*len += packet_write_uint16(buffer + *len, h->hp);
	}

	if (h->flags_hero != h->ss->flags_hero || full_update)
	{
		h->flags_world_snapshot |= FLAG_WORLD_SNAPSHOT_FLAGS_HERO;
		*len += packet_write_uint16(buffer + *len, h->flags_hero);
	}
	
	//cds could maybe be optimized if need be? only send every X updates to keep in sync?
	
	uint8_t pos = 10; // 
	for (int i = 0; i < SPELL_SLOTS; i++)
	{
		int32_t flag = 1 << (pos);
		h->flags_world_snapshot |= flag;
		*len += packet_write_uint16(buffer + *len, h->cd_spells[i]);
		pos++;
	}
	
	packet_write_uint16(buffer + len_f, h->flags_world_snapshot);
	
	//buffs and debuffs go here soon bruh
}

static void
room_snapshot_world(struct room *room, char *buffer, uint16_t *len)
{
	// game points and all that shit here bitch
}

static void
room_world_snapshot(struct room *room, bool full_update)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{	
		char buffer[PACKET_BUF_SIZE];
		
		struct client *cl = room->clients[i];
		if (cl == NULL) continue;

		for (uint16_t i = 0; i < sizeof(cl->hero->flags_world_snapshot) * 8; i++) 
		cl->hero->flags_world_snapshot &= ~(i);
		
		uint16_t len = 2; 
		room_snapshot_enemies(room, cl->hero, buffer, &len, full_update);
		room_snapshot_local(room, cl->hero, buffer, &len, full_update);
		room_snapshot_allies(room, cl->hero, buffer, &len, full_update);
		request_world_snapshot(cl, buffer, len);
	}
}

void
room_update(struct room *room)
{
	if (room->status == ROOM_STATE_OPEN)
	{
		room->start_game_timer--;

		if (room->start_game_timer <= 0)
		{
			room_game_init(room);
			room->status = ROOM_STATE_CLOSED;
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				struct client *cl = room->clients[i];
				if (cl == NULL) continue;

				cl->hero = world_hero_get(&room->game->world, cl->info.id_account);
				request_room_game_start(cl, room);
			}
		}
	}
	else if (room->status == ROOM_STATE_CLOSED)
	{
		game_update(room->game);
		room_world_snapshot(room, true);
	}

}

bool
room_has_member(struct room *room, uint32_t id)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (room->clients[i] == NULL) continue;

		if (room->clients[i]->pm->id == id) return true;
	}

	return false;
}

int8_t
room_can_join(struct room *room, uint8_t party_count)
{
	//static bool jt0 = true;
	//
	//if (jt0)
	//{
	//	jt0 = false;
	//	return TEAM0;
	//}
	//else
	//{
	//	jt0 = true;
	//	return TEAM1;
	//}


	uint8_t t0_count = 0;
	uint8_t t1_count = 0;

	if (room->status == ROOM_STATE_CLOSED) return -1;
	
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct client *cl = room->clients[i];
		if (cl == NULL) continue;

		if (cl->pm->team == TEAM0)
		{
			t0_count++;
		}
		else if (cl->pm->team == TEAM1)
		{
			t1_count++;
		}
	}

	if (MAX_TEAM_SIZE - t0_count >= party_count)
	{
		return TEAM0;
	}
	else if (MAX_TEAM_SIZE - t1_count >= party_count)
	{
		return TEAM1;
	}

	return -1;
}

void
room_member_add(struct room *room, struct client *cl)
{
	cl->pm->ready = false;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (room->clients[i] == NULL)
		{	
			room->clients[i] = cl;
			room->member_count++;
			break;
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (room->clients[i] == NULL || room->clients[i]->pm->id == cl->pm->id) continue;

		request_room_add(room->clients[i], room, cl->pm->id);
	}

}

void
room_member_remove(struct room *room, uint32_t id)
{

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (room->clients[i] == NULL) continue;

		request_room_leave(room->clients[i], room, id);
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct client *cl = room->clients[i];
		if (cl == NULL) continue;


		if (cl->pm->id == id)
		{
			if (room->status == ROOM_STATE_CLOSED)
			{
				world_hero_remove(&room->game->world, world_hero_get(&room->game->world, id));
				cl->hero = NULL;
				*cl->pm = party_member_init(
					cl->info.id_account, 
					cl->info.username, 
					cl->info.name_color.c);
			}

			room->clients[i] = NULL;

			room->member_count--;	
			break;
		}
	}
}

void
room_game_init(struct room *room)
{
	room->game = game_init();

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct client *cl = room->clients[i];
		if (cl == NULL) continue;

		struct hero *hero = world_hero_slot(&room->game->world); 
		hero_init(hero, &room->game->world, cl->pm->id_hero, cl->pm->id, cl->pm->team);
		hero_init_username(hero, cl->pm->username, cl->pm->username_color);

		world_hero_init(&room->game->world, hero);

		cl->hero = hero;
	}
}


