#ifndef PACKET_PROCESSOR__
#define PACKET_PROCESSOR__

#include "server.h"
#include "client.h"
#include "../server_manager.h"

void
process_packet(struct packet *packet, struct client *cl);

#endif

