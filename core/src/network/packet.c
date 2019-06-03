#include <limits.h>
#include "packet.h"
#include "../util/log.h"
#include "../util/game_values.h"

uint16_t 
packet_write_char(char *buffer, const char *data, uint16_t len)
{
	memcpy(buffer, data, len);
	return len;
}

uint16_t 
packet_read_char(char *data, const char *buffer, uint16_t len)
{
	memcpy(data, buffer, len);
	return len;
}

uint8_t 
packet_write_uint8(char *buffer, uint8_t data)
{
	uint8_t len = sizeof(uint8_t);
	memcpy(buffer, &data, len);
	return len;
}

uint8_t 
packet_read_uint8(uint8_t *data, const char *buffer)
{
	uint8_t len = sizeof(uint8_t);
	memcpy(data, buffer, len);
	return len;
}

uint8_t 
packet_write_int8(char *buffer, int8_t data)
{
	uint8_t len = sizeof(int8_t);
	data = (int8_t)data;
	memcpy(buffer, &data, len);
	return len;
}

uint8_t 
packet_read_int8(int8_t *data, const char *buffer)
{
	uint8_t len = sizeof(int8_t);
	memcpy(data, buffer, len);
	return len;
}

uint8_t 
packet_write_uint16(char *buffer, uint16_t data)
{
	uint8_t len = sizeof(uint16_t);
	data = htons(data);
	memcpy(buffer, &data, len);
	return len;
}

uint8_t 
packet_read_uint16(uint16_t *data, const char *buffer)
{
	uint8_t len = sizeof(uint16_t);
	uint16_t data_ntohs;
	memcpy(&data_ntohs, buffer, len);
	*data = ntohs(data_ntohs);
	return len;
}

uint8_t 
packet_write_int16(char *buffer, int16_t data)
{
	uint8_t len = sizeof(uint16_t);
	data = htons((uint16_t)data);
	memcpy(buffer, &data, len);
	return len;
}

uint8_t 
packet_read_int16(int16_t *data, const char *buffer)
{
	uint8_t len = sizeof(uint16_t);
	uint16_t data_ntohs;
	memcpy(&data_ntohs, buffer, len);
	*data = ntohs((int16_t)data_ntohs);
	return len;
}

uint8_t 
packet_write_uint32(char *buffer, uint32_t data)
{
	uint8_t len = sizeof(uint32_t);
	data = htonl(data);
	memcpy(buffer, &data, len);
	return len;
}

uint8_t 
packet_write_float(char *buffer, float data)
{
	return packet_write_int32(buffer, (int32_t)(data*FLOAT_MUL));
}

uint8_t 
packet_read_uint32(uint32_t *data, const char *buffer)
{
	uint8_t len = sizeof(uint32_t);
	uint32_t data_ntohl;
	memcpy(&data_ntohl, buffer, len);
	*data = ntohl(data_ntohl);
	return len;
}

uint8_t 
packet_write_int32(char *buffer, int32_t data)
{
	uint8_t len = sizeof(uint32_t);
	data = htonl((uint32_t)data);
	memcpy(buffer, &data, len);
	return len;
}

uint8_t 
packet_read_int32(int32_t *data, const char *buffer)
{
	uint8_t len = sizeof(uint32_t);
	uint32_t data_ntohl;
	memcpy(&data_ntohl, buffer, len);
	*data = ntohl((int32_t)data_ntohl);
	return len;
}

uint8_t 
packet_read_float(float *data, const char *buffer)
{
	uint8_t len = sizeof(uint32_t);
	uint32_t data_ntohl;
	memcpy(&data_ntohl, buffer, len);
	int32_t i_data = ntohl((int32_t)data_ntohl);
	float f_data = (float)i_data / FLOAT_MUL;
	*data = f_data;

	return len;
}

void
packet_init(
		struct packet_buffer *pack_buf,
		const char *buffer, 
		uint16_t packet_len,
		bool reliable,
		enum packet_type type)
{
	
	struct packet *packet = NULL;
	for (int i = 0; i < PACKET_ARR_SIZE; i++)
	{
		if (pack_buf->write[i].type == PACKET_INVALID) packet = &pack_buf->write[i];
	}

	if (packet == NULL) log_warn("Out of packet space on write arr!");

	packet->len = packet_len;
	packet->reliable = reliable;
	packet->sequence = pack_buf->sequence;
	packet->type = type;

	if (type != PACKET_ACK) pack_buf->sequence++;

	type = (uint8_t) type;
		
	uint16_t key = GAME_KEY;
	uint16_t len = 0;
	
	len += packet_write_uint16(packet->buffer + len, key);
	len += packet_write_uint8(packet->buffer + len, reliable);
	len += packet_write_uint8(packet->buffer + len, packet->sequence);
	len += packet_write_uint8(packet->buffer + len, type);

	len += packet_write_char(packet->buffer + len, buffer, packet_len);
	
	if (pack_buf->write_history_count == PACKET_ARR_SIZE) pack_buf->write_history_count = 0;
	pack_buf->write_history[pack_buf->write_history_count] = *packet;
	pack_buf->write_history_count++;

	pack_buf->write_count++;

}

struct packet_buffer*
packet_buffer_init(void)
{
	struct packet_buffer *buf = malloc(sizeof(struct packet_buffer));
	buf->sequence = 0;
	buf->sequence_read_last = 0;
	buf->write_count = 0;
	buf->write_history_count = 0;
	buf->timer_reliable_retry = 0;
	buf->write_attempts = 0;

	for (int i = 0; i < PACKET_ARR_SIZE; i++)
	{
		buf->write[i].type = PACKET_INVALID;	
		buf->write_history[i].type = PACKET_INVALID;	
		buf->read[i].type = PACKET_INVALID;	
	}

	return buf;
}

static void
packet_ack(struct packet_buffer *pack_buf, uint8_t seq)
{
	for (int i = 0; i < PACKET_ARR_SIZE; i++)
	{
		struct packet *p = &pack_buf->write[i]; 
		if (p->type == PACKET_INVALID) continue;

		if (p->sequence == seq) p->type = PACKET_INVALID;
	}
}

static void
packet_ack_send(struct packet_buffer *pack_buf, uint8_t seq)
{
	char buffer[PACKET_BUF_SIZE];
	uint16_t len = 0;

	len += packet_write_uint8(buffer + len, seq);
	
	packet_init(pack_buf, buffer, len, false, PACKET_ACK);
}

struct packet*
packet_read(struct packet_buffer *pack_buf, char *buffer, int32_t res)
{	
	if (pack_buf->read_count == PACKET_ARR_SIZE) pack_buf->read_count = 0;
	struct packet *p = &pack_buf->read[pack_buf->read_count];

	uint16_t key = 0;
	uint8_t reliable = 0;
	uint8_t seq = 0;
	uint8_t type = 0;

	uint16_t len = 0;
	
	len += packet_read_uint16(&key, buffer + len);
	len += packet_read_uint8(&reliable, buffer + len);
	len += packet_read_uint8(&seq, buffer + len);
	len += packet_read_uint8(&type, buffer + len);

	p->reliable = reliable;
	p->sequence = seq;
	p->type = type;

	//log_trace("Packet seq: %u Type %d || Packet_buf seq %u", p->sequence, p->type, pack_buf->sequence_read_last);
	//if (p->sequence != 0)
	//{
	//	if (p->sequence-1 != pack_buf->sequence_read_last)
	//	{
	//		log_trace(
	//				"Dropped packet! Packet seq: %u || Last read seq: %u", 
	//				p->sequence, 
	//				pack_buf->sequence_read_last);
	//	}
	//}

	if (key != GAME_KEY) p->type = PACKET_INVALID;
	
	p->len = (uint16_t)(res - HEADER_SIZE); 
	
	memcpy(p->buffer, buffer + HEADER_SIZE, p->len);

	if (p->reliable) packet_ack_send(pack_buf, p->sequence);

	if(p->type != PACKET_ACK)
	{

	//	log_trace(
	//			"Type: %d Seq: %d Rel %d Buf seq: %d", 
	//			p->type, 
	//			p->sequence, 
	//			p->reliable,
	//			pack_buf->sequence_read_last);

		int16_t s = p->sequence;
		uint8_t l = pack_buf->sequence_read_last;
		
		if (s - l < -127) s += 256;
		else if (s - l > 127) s -= 256;
	
		if (s > l)
		{
			if (s - l > 31)
			{
				pack_buf->ack_bit_field = 1;
			}
			else
			{
				pack_buf->ack_bit_field <<= (s - l);
				pack_buf->ack_bit_field |= 1;
			}

			pack_buf->sequence_read_last = s & 255;
		}
		else if (l - s < 32)
		{
			bool dup = (pack_buf->ack_bit_field & (1 << (l - s))) != 0;
			pack_buf->ack_bit_field |= (1 << (l - s));

			if (dup)
			{
				log_trace("Packet dup seq %d type %d", p->sequence, p->type);
				p->type = PACKET_INVALID;
			}
		}
		
	}
	else
	{
		uint8_t seq = 0;
		packet_read_uint8(&seq, p->buffer);
		packet_ack(pack_buf, seq);
	}

	pack_buf->read_count++;	

	return p;
}

void
packet_buffer_write(struct packet_buffer *pack_buf, int32_t fd, struct sockaddr_in addr)
{

	if (pack_buf->write_attempts >= PACKET_WRITE_MAX_ATTEMPTS) return;

	pack_buf->timer_reliable_retry++;

	for (int i = PACKET_ARR_SIZE; i--;)
	{
		struct packet *packet = &pack_buf->write[i];
		if (packet->type == PACKET_INVALID) continue;
		bool write = true;
		
		if (packet->reliable)
		{
			if (pack_buf->timer_reliable_retry < PACKET_RESEND_WAIT_TIME) write = false;	
		}
		else
		{	
			packet->type = PACKET_INVALID;
			pack_buf->write_count--;
		}
		
		if (write)
		{
			int32_t res = sendto(
					fd, 
					packet->buffer, 
					packet->len + HEADER_SIZE, 
					0, 
					(const struct sockaddr*)&addr, 
					sizeof(addr));

			if (res == -1) pack_buf->write_attempts++;

			//log_trace("Wrote %d bytes", res);
		}

	}

	if (pack_buf->timer_reliable_retry >= PACKET_RESEND_WAIT_TIME) 
		pack_buf->timer_reliable_retry = 0;
}

void
packet_sort_order(struct packet_buffer *pack_buf)
{
	bool swapped = false;

	for (int i = 0; i < PACKET_ARR_SIZE-1; i++)
	{
		struct packet *pi = &pack_buf->read[i];
		if (pi->type == PACKET_INVALID) continue;
		
		swapped = false;

		for (int j = 0; j < PACKET_ARR_SIZE-1-i; j++)
		{

			struct packet *p = &pack_buf->read[j];
			struct packet *p1 = &pack_buf->read[j+1];
			struct packet tmp;
			
			if (p->type == PACKET_INVALID || p1->type == PACKET_INVALID) continue;

			if (p->sequence > p1->sequence)
			{
				tmp = *p;
				*p = *p1;
				*p1 = tmp;
				swapped = true;
			}
		}

		if (!swapped) break;
	}
}

uint16_t
packet_get_key(const struct packet *packet)
{
	uint16_t key = 0;	

	packet_read_uint16(&key, packet->buffer);

	return key;
}

uint8_t
packet_buffer_get_type(char *buffer)
{
	uint8_t type = 0;	

	packet_read_uint8(&type, buffer + 4);

	return type;
}

struct packet*
packet_get_write_history(struct packet_buffer *pack_buf, uint32_t seq)
{
		
	return NULL;
}

struct packet*
packet_get_read(struct packet_buffer *pack_buf, uint32_t seq)
{
		
	return NULL;
}





