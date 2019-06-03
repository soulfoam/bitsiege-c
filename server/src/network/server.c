#include <time.h>
#include "server.h"
#include "packet_processor.h"
#include "util/util_funcs.h"
#include "util/log.h"

static struct server sv;

struct server*
server(void)
{
	return &sv;
}

void
server_read(void)
{
	
	struct sockaddr_in addr_from;
	socklen_t addr_len = sizeof(addr_from);

	char buffer[PACKET_BUF_SIZE];

	int32_t res = recvfrom(
			sv.fd, 
			buffer, 
			PACKET_BUF_SIZE, 
			0, 
			(struct sockaddr*) 
			&addr_from, 
			&addr_len);

	if (res == -1) return;
	
	//log_trace(
	//		"Packet read from %s || Reliable %u || Seq: %u \
	//		|| Read: %d || Packet Len: %u || Type: %u", 
	//		inet_ntoa(addr_from.sin_addr), 
	//		packet.reliable, 
	//		packet.sequence, 
	//		res,
	//		packet.len,
	//		packet.type);


	//if (packet.type == PACKET_INVALID)
	//{
	//	log_warn(
	//			"Packet from %s:%d has invalid game key... %u", 
	//			inet_ntoa(addr_from.sin_addr), 
	//			ntohs(addr_from.sin_port), 
	//			packet_get_key(&packet));
	//	return;
	//}

	struct client *cl = client_get(&addr_from);	
	if (cl == NULL) cl = client_init(addr_from);

	cl->last_time_packet = time(NULL);
	cl->need_ping = false;

	struct packet *p = packet_read(cl->pack_buf, buffer, res);
	if (p->type == PACKET_PING) cl->last_time_ping_packet = get_time_ms();

}

void
server_process(void)
{

	for (int i = 0; i < MAX_USERS; i++)
	{
		struct client *cl = &sv.clients[i];
		if (cl->pack_buf == NULL) continue;

		for (int j = 0; j < PACKET_ARR_SIZE; j++)
		{
			struct packet *p = &cl->pack_buf->read[j];
			if (p->type != PACKET_INVALID)
			{
				process_packet(p, cl);
				p->type = PACKET_INVALID;
			}
		}	

		client_update(cl);
	}
}

void
server_write_packets(void)
{	
	for (int i = 0; i < MAX_USERS; i++)
	{
		struct client *cl = &sv.clients[i];
		if (cl->pack_buf == NULL) continue;

		packet_buffer_write(cl->pack_buf, sv.fd, cl->addr_cl);
	}
}

void
server_init(void)
{
	sv.fd = -1;
	sv.client_count = 0;
	
	for (int i = 0; i < MAX_USERS; i++) sv.clients[i].pack_buf = NULL;
		
	if ((sv.fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Failed to create socket");
		log_fatal("Failed to create socket");
	}
	
	memset(&sv.addr_sv, 0, sizeof(sv.addr_sv));

	sv.addr_sv.sin_family = AF_INET;
	sv.addr_sv.sin_addr.s_addr = INADDR_ANY;
	sv.addr_sv.sin_port = htons(NETWORK_PORT);

	if (bind(sv.fd, (const struct sockaddr*)&sv.addr_sv, sizeof(sv.addr_sv)) < 0)
	{
		perror("Failed to bind socket.");
		log_fatal("Failed to bind socket.");
	}


	int32_t rc;
	int32_t on = 1;
	rc = setsockopt(
			sv.fd, 
			SOL_SOCKET, 
			SO_REUSEADDR, 
			(char *)&on, 
			sizeof(on));

	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(sv.fd);
		exit(-1);
	}

	rc = ioctl(sv.fd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(sv.fd);
		exit(-1);
	}

	log_info("Network initialized.");
}


