#ifndef CLIENT__
#define CLIENT__

#include <errno.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include "network/packet.h"
#include "../engine.h"

#ifndef _WIN32
typedef int16_t sock;
#else
typedef SOCKET sock;
#endif


#define BUFFER_WORLD_SNAPSHOT_DELAY 1 //50ms
#define BUFFER_WORLD_SNAPSHOT_SIZE (TICKS_PER_SECOND / 4)

struct 
client 
{
	sock fd;
	struct packet_buffer *pack_buf; 
	bool connected;

	struct sockaddr_in addr_sv;

	struct command ss_buffer[BUFFER_WORLD_SNAPSHOT_SIZE];
	uint8_t ss_buffer_count;
	uint16_t ss_buffer_timer;
	bool ss_got_first;
};

struct client*
client(void);

void
client_disconnect(void);

void
packet_write(struct packet *packet);

void
client_read(void);

void
client_process(void);

void
client_write_packets(void);

int
client_init(void);

void
client_destroy(void);


#endif
