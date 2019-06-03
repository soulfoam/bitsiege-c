#include "client.h"
#include "packet_processor.h"
#include "client_packet.h"
#include "util/log.h"
#include "util/util_funcs.h"

static struct client cl;

struct client*
client(void)
{
	return &cl;
}

void
client_disconnect(void)
{
	request_disconnect();
//#ifndef _WIN32
//	close(cl.fd);	
//#else
//	closesocket(cl.fd);
//	WSACleanup();
//#endif
//	cl.connected = false;
}

void 
client_read(void) 
{	
	char buffer[PACKET_BUF_SIZE];
	socklen_t addr_len = sizeof(cl.addr_sv);

	int32_t res = recvfrom(
			cl.fd, 
			buffer, 
			PACKET_BUF_SIZE, 
			0, 
			(struct sockaddr*) 
			&cl.addr_sv, 
			&addr_len);

	if (res == -1) return;
	
	packet_read(cl.pack_buf, buffer, res);
}

static void
client_world_snapshot_handle(void)
{
	cl.ss_buffer_timer++;
	if (cl.ss_buffer_timer >= BUFFER_WORLD_SNAPSHOT_DELAY && cl.ss_got_first)
	{
		if (cl.ss_buffer[0].processed) return; // extrapolate here

		cl.ss_buffer_timer = 0;	

		command_init_world_snapshot(game_command_init(eng()->game), cl.ss_buffer[0].data);

		cl.ss_buffer[0].processed = true;

		for (int i = 0; i < client()->ss_buffer_count; i++)
		{
			if (i == client()->ss_buffer_count-1) continue;
			client()->ss_buffer[i] = client()->ss_buffer[i+1];
		}
		
		cl.ss_buffer_count--;

	}
	
}

void
client_process(void)
{
	packet_sort_order(cl.pack_buf);

	for (int i = 0; i < PACKET_ARR_SIZE; i++)
	{
		struct packet *p = &cl.pack_buf->read[i];
		if (p->type != PACKET_INVALID)
		{
			process_packet(p);
			p->type = PACKET_INVALID;
		}
	}
	
	client_world_snapshot_handle();
}

void
client_write_packets(void)
{
	packet_buffer_write(cl.pack_buf, cl.fd, cl.addr_sv);		
}

int 
client_init(void) 
{
#ifdef _WIN32
	WSADATA wsa_data;
	int32_t res_startup = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (res_startup != NO_ERROR)
	{
		 log_warn("Win32 socket failed to start.");	
		 return false;
	}
#endif

	cl.connected = false;
	cl.ss_buffer_count = 0;
	cl.ss_buffer_timer = 0;
	cl.ss_got_first = false;
	for (int i = 0; i < BUFFER_WORLD_SNAPSHOT_SIZE; i++) client()->ss_buffer[i].processed = true;
		
	cl.pack_buf = packet_buffer_init();

	cl.fd = socket(AF_INET, SOCK_DGRAM, 0);

#ifndef _WIN32
if (cl.fd == -1) 
{
	log_warn("Linux failed to create socket.");
	return false;
}
#else
if (cl.fd == INVALID_SOCKET) 
{
	client_disconnect();
	log_warn("Win32 failed to create socket.");
	return false;
}
#endif

	memset(&cl.addr_sv, 0, sizeof(cl.addr_sv));
	cl.addr_sv.sin_family = AF_INET;
	cl.addr_sv.sin_port = htons(NETWORK_PORT);
	
	if (inet_pton(AF_INET, SERVER_IP_2, &cl.addr_sv.sin_addr) == 0)
	{
		log_trace("inet aton failed to set server ip.");
	}

#ifndef _WIN32
	int32_t rc;
	int32_t on = 1;
	rc = setsockopt(
			cl.fd, 
			SOL_SOCKET, 
			SO_REUSEADDR, 
			(char *)&on, 
			sizeof(on));

	rc = ioctl(cl.fd, FIONBIO, (char *)&on);

#else
	unsigned long mode = 1;
	int32_t res_ioctl = ioctlsocket(cl.fd, FIONBIO, &mode);
	if (res_ioctl != NO_ERROR) 
	{
		log_warn("Win32 ioctl failed.");
		client_disconnect();
		return false;
	}
#endif

	log_info("Network initialized.");

	return true;
}

void 
client_destroy(void) 
{
	if (cl.fd) 
	{
		close(cl.fd);
		free(cl.pack_buf);
	}
}

