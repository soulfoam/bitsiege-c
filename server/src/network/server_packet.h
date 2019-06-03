#ifndef SERVER_PACKET__
#define SERVER_PACKET__

#include <inttypes.h>
#include "network/packet.h"
#include "network/packet_result_code.h"
#include "network/party.h"
#include "client.h"
#include "network/friend.h"

void
request_ping(struct client *cl);

void
request_login_result(
		struct client *cl, 
		enum packet_result result, 
		struct friend_list *f_list);

void
request_register_result(
		struct client *cl, 
		enum packet_result result);


void
request_party_invite(struct client *cl, struct client_info *inviter, uint16_t id_party);

void
request_party_add(struct client *cl, struct party *party, uint32_t id);

void
request_party_leave(struct client *cl, struct party *party, uint32_t id);

void
request_party_join(struct client *cl, struct party *party);



void
request_room_join(struct client *cl, struct room *room);

void
request_room_add(struct client *cl, struct room *room, uint32_t id);

void
request_room_leave(struct client *cl, struct room *room, uint32_t id);

void
request_room_game_start(struct client *cl, struct room *room);

void
request_room_hero_change(struct client *cl, struct room *room);


void
request_world_snapshot(struct client *cl, char *buffer, uint16_t len);




void
request_input(struct client *cl);

#endif

