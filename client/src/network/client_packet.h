#ifndef CLIENT_PACKET__
#define CLIENT_PACKET__

#include <inttypes.h>
#include "util/game_util.h"
#include "network/packet.h"
#include "game/hero/hero_info.h"
#include "network/packet_result_code.h"

void
request_ping(void);

void
request_disconnect(void);

void
request_account_register(char *username, const char *password, char *email);

void
request_account_login(char *username, const char *password);

void
request_party_invite(uint32_t fd);

void
request_party_invite_accept(uint16_t id_party);

void
request_party_invite_decline(uint16_t id_party);

void
request_party_que(void);

void
request_input_update(uint16_t flags_input, enum direction dir);

void
request_spell_cast(uint8_t slot, uint32_t id_target);

#endif

