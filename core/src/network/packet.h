#ifndef PACKET__
#define PACKET__

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#ifndef _WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#define GAME_KEY 0xD34D

#define PACKET_KEY_SIZE 2
#define PACKET_IS_RELIABLE_SIZE 1
#define PACKET_SEQ_SIZE 1
#define PACKET_TYPE_SIZE 1

#define HEADER_SIZE \
(PACKET_KEY_SIZE + PACKET_IS_RELIABLE_SIZE + PACKET_SEQ_SIZE + PACKET_TYPE_SIZE) 

#define PACKET_BUF_SIZE 1024

#define PACKET_ARR_SIZE 128

#define PACKET_WRITE_MAX_ATTEMPTS 10

#define PACKET_RESEND_WAIT_TIME (TICKS_PER_SECOND / 2) //in ticks

#define FLOAT_MUL 1000

enum
packet_type
{
	PACKET_INVALID,

	PACKET_ACK,

	PACKET_PING,
	PACKET_DISCONNECT,

	PACKET_USER_REGISTER,
	PACKET_USER_LOGIN,

	PACKET_ROOM_JOIN,
	PACKET_ROOM_ADD,
	PACKET_ROOM_LEAVE,
	PACKET_ROOM_GAME_START,
	PACKET_ROOM_HERO_CHANGE,

	PACKET_PARTY_INVITE,
	PACKET_PARTY_INVITE_ACCEPT,
	PACKET_PARTY_INVITE_DECLINE,
	PACKET_PARTY_JOIN,
	PACKET_PARTY_ADD,
	PACKET_PARTY_LEAVE,
	PACKET_PARTY_KICK,
	PACKET_PARTY_QUE,

	PACKET_INPUT,

	PACKET_WORLD_SNAPSHOT,
	PACKET_WORLD_SPELL_CAST,
	PACKET_WORLD_SPELL_ADD
};

struct
packet
{
	char buffer[PACKET_BUF_SIZE];	
	uint16_t len;

	bool reliable; 
	uint8_t sequence; 
	enum packet_type type;
};

struct
packet_buffer
{
	struct packet write[PACKET_ARR_SIZE];
	uint16_t write_count;
	struct packet write_history[PACKET_ARR_SIZE];
	uint16_t write_history_count;
	uint8_t write_attempts; 

	uint16_t timer_reliable_retry;


	struct packet read[PACKET_ARR_SIZE];
	uint16_t read_count;
	
	uint8_t sequence;
	uint8_t sequence_read_last;
	uint32_t ack_bit_field;

};

uint16_t 
packet_write_char(char *buffer, const char *data, uint16_t len);

uint16_t 
packet_read_char(char *data, const char *buffer, uint16_t len);

uint8_t 
packet_write_uint8(char *buffer, uint8_t data);

uint8_t 
packet_read_uint8(uint8_t *data, const char *buffer);

uint8_t 
packet_write_int8(char *buffer, int8_t data);

uint8_t 
packet_read_int8(int8_t *data, const char *buffer);

uint8_t 
packet_write_uint16(char *buffer, uint16_t data);

uint8_t 
packet_read_uint16(uint16_t *data, const char *buffer);

uint8_t 
packet_write_int16(char *buffer, int16_t data);

uint8_t 
packet_read_int16(int16_t *data, const char *buffer);

uint8_t 
packet_write_uint32(char *buffer, uint32_t data);

uint8_t 
packet_write_float(char *buffer, float data);

uint8_t 
packet_read_uint32(uint32_t *data, const char *buffer);

uint8_t 
packet_write_int32(char *buffer, int32_t data);

uint8_t 
packet_read_int32(int32_t *data, const char *buffer);

uint8_t 
packet_read_float(float *data, const char *buffer);

void
packet_init(
		struct packet_buffer *pack_buf,
		const char *buffer, 
		uint16_t packet_len,
		bool reliable,
		enum packet_type type);

struct packet_buffer*
packet_buffer_init(void);

struct packet*
packet_read(struct packet_buffer *pack_buf, char *buffer, int32_t res);

void
packet_buffer_write(struct packet_buffer *pack_buf, int32_t fd, struct sockaddr_in addr);

void
packet_sort_order(struct packet_buffer *pack_buf);

uint16_t
packet_get_key(const struct packet *packet);

#endif

