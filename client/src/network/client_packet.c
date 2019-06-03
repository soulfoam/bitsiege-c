#include <math.h>
#include "client.h"
#include "client_packet.h"
#include "util/util_funcs.h"
#include "util/log.h"
#include "util/game_values.h"
#include "network/packet_result_code.h"

static void
request_packet(char *buffer, uint16_t len, bool reliable, enum packet_type type)
{
	packet_init(client()->pack_buf, buffer, len, reliable, type);
}

void
request_ping(void)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	request_packet(buffer, len, true, PACKET_PING);

//	client()->ping_timestamp = SDL_GetTicks();
}

void
request_disconnect(void)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	request_packet(buffer, len, true, PACKET_DISCONNECT);
}

void
request_account_register(char *username, const char *password, char *email)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;	

	len += packet_write_char(buffer + len, username, MAX_USERNAME_LEN);
	len += packet_write_char(buffer + len, password, MAX_PASSWORD_LEN);
	len += packet_write_char(buffer + len, email, MAX_EMAIL_LEN);
	
	request_packet(buffer, len, true, PACKET_USER_REGISTER);

}

void
request_account_login(char *username, const char *password)
{
	//username = trim_white_space(username);	

	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	len += packet_write_char(buffer + len, username, MAX_USERNAME_LEN);
	len += packet_write_char(buffer + len, password, MAX_PASSWORD_LEN);
	
	request_packet(buffer, len, true, PACKET_USER_LOGIN);
}

void
request_party_invite(uint32_t id)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	len += packet_write_uint32(buffer + len, id);

	request_packet(buffer, len, true, PACKET_PARTY_INVITE);
}

void
request_party_invite_accept(uint16_t id_party)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	len += packet_write_uint16(buffer + len, id_party);

	request_packet(buffer, len, true, PACKET_PARTY_INVITE_ACCEPT);
}

void
request_party_invite_decline(uint16_t id_party)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	len += packet_write_uint16(buffer + len, id_party);

	request_packet(buffer, len, true, PACKET_PARTY_INVITE_DECLINE);
}

void
request_party_que(void)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	request_packet(buffer, len, true, PACKET_PARTY_QUE);
}

void
request_input_update(uint16_t flags_input, enum direction dir)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	uint8_t d = (uint8_t) dir;
	
	len += packet_write_uint16(buffer + len, flags_input);
	len += packet_write_uint8(buffer + len, d);

	request_packet(buffer, len, false, PACKET_INPUT);
}

void
request_spell_cast(uint8_t slot, uint32_t id_target)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;
	
	len += packet_write_uint8(buffer + len, slot);
	len += packet_write_uint32(buffer + len, id_target);

	request_packet(buffer, len, true, PACKET_WORLD_SPELL_CAST);
}


