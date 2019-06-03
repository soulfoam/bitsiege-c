#ifndef SERVER__
#define SERVER__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include "network/packet.h"
#include "game/world.h"
#include "server_packet.h"
#include "client.h"
#include "../server_manager.h"
#include "server_funcs.h"

struct server
{
	int32_t fd;

	uint32_t client_count;
	struct client clients[MAX_USERS];

	struct sockaddr_in addr_sv;

};

void
server_init(void);

void 
server_read(void);

void 
server_process(void);

void
server_write_packets(void);

void
server_remove_client(int32_t id_index, int32_t client_index);

struct server*
server(void);

#endif

