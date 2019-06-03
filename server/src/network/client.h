#ifndef CLIENT__
#define CLIENT__

#include <inttypes.h>
#include <stdbool.h>
#include <sys/poll.h>
#include <netinet/ip.h>
#include "network/packet.h"
#include "game/hero/hero.h"
#include "../room/room.h"
#include "network/party.h"

struct client_info
{	
	uint32_t id_account;
	char username[MAX_USERNAME_LEN+1];
	char email[MAX_EMAIL_LEN+1];
	bool logged_in;
	union color name_color;
};

struct client
{
	struct sockaddr_in addr_cl;
	
	struct packet_buffer *pack_buf;
	struct client_info info;

	struct party *party;
	struct party_member *pm;
	struct room *room;
	struct hero *hero;

	time_t last_time_packet;
	bool need_ping;

	uint16_t ping;
	long long int last_time_ping_packet;

	bool disconnect;

}; 

struct client*
client_init(struct sockaddr_in addr_cl);

void
client_remove(struct client *cl);

bool
sockaddr_in_match(
		struct sockaddr_in *addr, 
		struct sockaddr_in *addr2);

struct client*
client_get(struct sockaddr_in *addr);

void
client_update(struct client *cl);

struct client*
client_get_id(uint32_t db_id);

bool
client_in_game(struct client *cl);

const char*
client_ip(const struct client *cl);

uint16_t
client_port(const struct client *cl);

#endif

