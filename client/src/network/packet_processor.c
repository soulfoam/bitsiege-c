#include "packet_processor.h"
#include "client_packet.h"
#include "network/packet_result_code.h"
#include "../engine.h"
#include "client.h"
#include "util/log.h"
#include "network/party.h"

static void
process_register(struct packet *packet)
{
	uint16_t len = 0;
	
	int32_t result = 0;

	len += packet_read_int32(&result, packet->buffer + len);

	log_trace("register result: %u", result);

}

static void
process_login(struct packet *packet)
{
	uint16_t len = 0;

	int32_t result;

	len += packet_read_int32(&result, packet->buffer + len);

	if (result == RESULT_USER_LOGIN_SUCCESS)
	{	
		
		len += packet_read_uint32(&eng()->acc_info.id, packet->buffer + len);
		uint8_t friend_count;

		len += packet_read_uint8(&friend_count, packet->buffer + len);

		struct friend_list list = friend_list_init();	

		for (int i = 0; i < friend_count; i++)
		{
			struct friend fr;

			uint32_t id;
			char username[MAX_USERNAME_LEN+1];
			uint32_t name_color;
			uint8_t online;

			len += packet_read_uint32(&id, packet->buffer + len);
			len += packet_read_char(username, packet->buffer + len, MAX_USERNAME_LEN);
			len += packet_read_uint32(&name_color, packet->buffer + len);
			len += packet_read_uint8(&online, packet->buffer + len);

			fr = friend_init(id, username, online);
			fr.name_color.c = name_color;

			log_trace("Got friend %s %u online: %d", username, name_color, online);

			friend_list_add(&list, fr);
		}	

		eng()->acc_info.f_list = list;
	}

}

static void
process_party_invite(struct packet *packet)
{
	uint16_t len = 0; 

	uint16_t id_party;
	char name[MAX_USERNAME_LEN+1];
	union color nc;

	len += packet_read_uint16(&id_party, packet->buffer + len);
	len += packet_read_char(name, packet->buffer + len, MAX_USERNAME_LEN);
	len += packet_read_uint32(&nc.c, packet->buffer + len);

	log_trace("Got party invite from %s (%d) to party: %d", name, nc.c, id_party);
}

static void
process_party_join(struct packet *packet)
{
	uint16_t len = 0; 

	uint8_t party_count;
	len += packet_read_uint8(&party_count, packet->buffer + len);

	for (int i = 0; i < party_count; i++)
	{
		uint32_t id = UINT_MAX; 
		char *name = NULL;
		uint32_t c;

		len += packet_read_uint32(&id, packet->buffer + len);
		len += packet_read_char(name, packet->buffer + len, MAX_USERNAME_LEN);
		len += packet_read_uint32(&c, packet->buffer + len);

		log_trace("Joined party: %s (%d) id: %d", name, c, id);
	}

}

static void
process_party_leave(struct packet *packet)
{

}

static void
process_room_join(struct packet *packet)
{
	uint16_t len = 0;	

	uint8_t member_count = 0;

	len += packet_read_uint8(&member_count, packet->buffer + len);
	for (int i = 0; i < member_count; i++)
	{
		
		uint32_t id = UINT_MAX;
		char username[MAX_USERNAME_LEN];
		uint32_t c;
		uint8_t team = TEAM0;
		uint8_t ready = 0;

		len += packet_read_uint32(&id, packet->buffer + len);
		len += packet_read_char(username, packet->buffer + len, MAX_USERNAME_LEN);
		len += packet_read_uint32(&c, packet->buffer + len);
		len += packet_read_uint8(&team, packet->buffer + len);
		len += packet_read_uint8(&ready, packet->buffer + len);

		struct party_member pm = party_member_init(id, username, c);
		pm.team = team;
		pm.ready = ready;
		
		log_trace("Got add member in room: fd: %d, name: %s, color: %u team: %d", 
				pm.id, 
				pm.username, 
				pm.username_color.c,
				pm.team);
		
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (eng()->info.room_members[i].id == UINT_MAX)
			{
				eng()->info.room_members[i] = pm;
				break;
			}
		}
	}

}

static void
process_room_add(struct packet *packet)
{

	uint16_t len = 0;	

	uint32_t id = UINT_MAX;
	char username[MAX_USERNAME_LEN];
	uint32_t c;
	uint8_t team = TEAM0;

	len += packet_read_uint32(&id, packet->buffer + len);
	len += packet_read_char(username, packet->buffer + len, MAX_USERNAME_LEN);
	len += packet_read_uint32(&c, packet->buffer + len);
	len += packet_read_uint8(&team, packet->buffer + len);

	struct party_member pm = party_member_init(id, username, c);
	pm.team = team;

	
	log_trace("Got add member in room: fd: %d, name: %s, color: %u team: %d", 
			pm.id, 
			pm.username, 
			pm.username_color.c,
			pm.team);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (eng()->info.room_members[i].id == UINT_MAX)
		{
			eng()->info.room_members[i] = pm;
			break;
		}
	}
}

static void
process_room_leave(struct packet *packet)
{

	uint16_t len = 0;	
	uint32_t id = UINT_MAX;

	len += packet_read_uint32(&id, packet->buffer + len);
	
	log_trace("Got remove from room: %d", id);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (eng()->info.room_members[i].id == UINT_MAX) continue;

		if (eng()->info.room_members[i].id == id)
		{
			eng()->info.room_members[i].id = UINT_MAX;
			break;
		}
	}

	struct renderable_hero *rh = render_hero_get(id);
	rh->render_data.spr.visible = false;
	world_hero_remove(&eng()->game->world, rh->hero);

}

static void
process_room_game_start(struct packet *packet)
{
	if (eng()->info.in_game) return;
	
	game_destroy(eng()->game);
	eng()->game = game_init();

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct party_member *pm = &eng()->info.room_members[i];
		if (pm->id == UINT_MAX) continue;

		struct hero *hero = world_hero_slot(&eng()->game->world); 
		hero_init(hero, &eng()->game->world, pm->id_hero, pm->id, pm->team);
		hero_init_username(hero, pm->username, pm->username_color);
		hero->networked = true;
		
		world_hero_init(&eng()->game->world, hero);

		struct renderable_hero *ren_hero = render_hero_init(hero);

		if (hero->id_game == eng()->acc_info.id)
		{
			ren_hero->render_data.spr.visible = true;
			camera_follow_hero(
				ren_hero, 
				renderable_hero_camera_offset(ren_hero));

			eng()->local_hero = ren_hero;
		}
	}
	
	menu_state_set(MENU_STATE_PLAY);
		
	eng()->info.in_game = true;

	client()->ss_got_first = false;
	for (int i = 0; i < BUFFER_WORLD_SNAPSHOT_SIZE; i++) client()->ss_buffer[i].processed = true;
}

static void
process_room_hero_change(struct packet *packet)
{
	uint16_t len = 0;

	uint32_t id;
	len += packet_read_uint32(&id, packet->buffer + len);

	uint16_t id_hero;
	len += packet_read_uint16(&id_hero, packet->buffer + len);

	// just need to change ui here

}

static void
process_world_snapshot(struct packet *packet)
{
	
	if (!eng()->info.in_game) return;

	struct command_world_snapshot ss;

	uint16_t len = 0;

	len += packet_read_uint8(&ss.count_update_ally, packet->buffer + len);
	len += packet_read_uint8(&ss.count_update_enemy, packet->buffer + len);

	struct command_update_ally cua[ss.count_update_ally];
	uint8_t cuac = 0;

	struct command_update_enemy cue[ss.count_update_enemy];
	uint8_t cuec = 0;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct renderable_hero *ren_hero = &renderer()->ren_heroes[i];
		if (ren_hero->hero == NULL) continue;

		ren_hero->render_data.spr.visible = false;
		
	}

	for (int i = 0; i < ss.count_update_enemy; i++)
	{	

		uint32_t id = 0;
		uint16_t flags_update = 0;
		float x = 0;
		float y = 0;
		uint16_t input = 0;
		uint8_t action = 0;
		uint8_t dir = 0;
		uint16_t hp_base = 0;
		uint16_t hp = 0;
		uint16_t flags_hero = 0;

		len += packet_read_uint32(&id, packet->buffer + len);
		len += packet_read_uint16(&flags_update, packet->buffer + len);

		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_X)
		len += packet_read_float(&x, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_Y)
		len += packet_read_float(&y, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_INPUT)
		len += packet_read_uint16(&input, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_ACTION)
		len += packet_read_uint8(&action, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_DIR)
		len += packet_read_uint8(&dir, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP_BASE)
		len += packet_read_uint16(&hp_base, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP)
		len += packet_read_uint16(&hp, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_FLAGS_HERO)
		len += packet_read_uint16(&flags_hero, packet->buffer + len);
		
	//	log_trace(
	//			"Enemy Update snapshot || ID: %u X: %d Y: %d Input: %u Dir: %u HP: %u",
	//			id,
	//			x,
	//			y,
	//			input,
	//			dir,
	//			hp);
		struct command_update_enemy ue;
		ue.id = id;
		ue.flags_update = flags_update;
		ue.pos = vec2_init(x, y);
		ue.input = input;
		ue.action = action;
		ue.dir = dir;
		ue.hp_base = hp_base;
		ue.hp = hp;
		ue.flags_hero = flags_hero;

		cue[cuec] = ue;
		cuec++;

		struct renderable_hero *rh = render_hero_get(id);
		rh->render_data.spr.visible = true;
	}

	for (int i = 0; i < 1; i++)	
	{

		uint32_t id = 0;
		uint16_t flags_update = 0;
		float x = 0;
		float y = 0;
		uint8_t action = 0;
		uint16_t hp_base = 0;
		uint16_t hp = 0;
		uint16_t flags_hero = 0;
		uint16_t cd0 = 0;
		uint16_t cd1 = 0;
		uint16_t cd2 = 0;
		uint16_t cd3 = 0;
		uint16_t cd4 = 0;

		len += packet_read_uint32(&id, packet->buffer + len);
		len += packet_read_uint16(&flags_update, packet->buffer + len);

		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_X)
		len += packet_read_float(&x, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_Y)
		len += packet_read_float(&y, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_ACTION)
		len += packet_read_uint8(&action, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP_BASE)
		len += packet_read_uint16(&hp_base, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP)
		len += packet_read_uint16(&hp, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_FLAGS_HERO)
		len += packet_read_uint16(&flags_hero, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD0)
		len += packet_read_uint16(&cd0, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD1)
		len += packet_read_uint16(&cd1, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD2)
		len += packet_read_uint16(&cd2, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD3)
		len += packet_read_uint16(&cd3, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_CD4)
		len += packet_read_uint16(&cd4, packet->buffer + len);


	//	log_trace(
	//		"Proccessing local snapshot || ID: %u X: %d Y: %d HP: %u",
	//		id,
	//		x,
	//		y,
	//		hp);
		
		struct command_update_local ul;
		ul.id = id;
		ul.flags_update = flags_update;
		ul.pos = vec2_init_f(x, y);
		ul.action = action;
		ul.hp_base = hp_base;
		ul.hp = hp;
		ul.flags_hero = flags_hero;
		ul.cd0 = cd0;
		ul.cd1 = cd1;
		ul.cd2 = cd2;
		ul.cd3 = cd3;
		ul.cd4 = cd4;
		
		ss.update_local = ul;

		struct renderable_hero *rh = render_hero_get(id);
		rh->render_data.spr.visible = true;

	}

	for (int i = 0; i < ss.count_update_ally; i++)
	{	
		uint32_t id = 0;
		uint16_t flags_update = 0;
		float x = 0;
		float y = 0;
		uint16_t input = 0;
		uint8_t action = 0;
		uint8_t dir = 0;
		uint16_t hp_base = 0;
		uint16_t hp = 0;
		uint16_t flags_hero = 0;
		uint8_t has_ult = 0;

		len += packet_read_uint32(&id, packet->buffer + len);
		len += packet_read_uint16(&flags_update, packet->buffer + len);

		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_X)
		len += packet_read_float(&x, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_POS_Y)
		len += packet_read_float(&y, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_INPUT)
		len += packet_read_uint16(&input, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_ACTION)
		len += packet_read_uint8(&action, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_DIR)
		len += packet_read_uint8(&dir, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP_BASE)
		len += packet_read_uint16(&hp_base, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_HP)
		len += packet_read_uint16(&hp, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_FLAGS_HERO)
		len += packet_read_uint16(&flags_hero, packet->buffer + len);
		if (flags_update & FLAG_WORLD_SNAPSHOT_HAS_ULT)
		len += packet_read_uint8(&has_ult, packet->buffer + len);

	//	log_trace(
	//			"Ally Update snapshot || ID: %u X: %d Y: %d Input: %u Dir: %u HP: %u Ult: %u",
	//			id,
	//			x,
	//			y,
	//			input,
	//			dir,
	//			hp,
	//			has_ult);
		
		struct command_update_ally ua;
		ua.id = id;
		ua.flags_update = flags_update;
		ua.pos = vec2_init(x, y);
		ua.input = input;
		ua.action = action;
		ua.dir = dir;
		ua.hp_base = hp_base;
		ua.hp = hp;
		ua.flags_hero = flags_hero;
		ua.has_ult = has_ult;

		cua[cuac] = ua;
		cuac++;

		struct renderable_hero *rh = render_hero_get(id);
		rh->render_data.spr.visible = true;

	}	

	memcpy(ss.update_enemies, cue, sizeof(struct command_update_enemy) * ss.count_update_enemy);
	memcpy(ss.update_allies, cua, sizeof(struct command_update_ally) * ss.count_update_ally);
		
	command_init_world_snapshot(&client()->ss_buffer[client()->ss_buffer_count], &ss);
		
	client()->ss_buffer_count++;

	if (client()->ss_buffer_count >= 2) client()->ss_got_first = true;

	if (client()->ss_buffer_count == BUFFER_WORLD_SNAPSHOT_SIZE) 
		client()->ss_buffer_count = BUFFER_WORLD_SNAPSHOT_SIZE-1;

}

void
process_packet(struct packet *packet)
{
	
	if (packet == NULL) return;

	switch (packet->type)
	{
		case PACKET_USER_REGISTER:
			process_register(packet);
		break;

		case PACKET_USER_LOGIN:
			process_login(packet);
		break;

		case PACKET_PARTY_INVITE:
			process_party_invite(packet);
		break;

		case PACKET_PARTY_JOIN:
			process_party_join(packet);
		break;

		case PACKET_ROOM_JOIN:
			process_room_join(packet);
		break;

		case PACKET_ROOM_ADD:
			process_room_add(packet);
		break;

		case PACKET_ROOM_LEAVE:
			process_room_leave(packet);
		break;

		case PACKET_ROOM_GAME_START:
			process_room_game_start(packet);
		break;

		case PACKET_ROOM_HERO_CHANGE:
			process_room_hero_change(packet);
		break;

		case PACKET_WORLD_SNAPSHOT:
			process_world_snapshot(packet);
		break;

		case PACKET_INPUT:
			//process_input(packet);
		break;
		
		default: break;
	}

}
