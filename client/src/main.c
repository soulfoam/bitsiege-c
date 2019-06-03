#include "engine.h"
#include "network/client.h"
#include "network/client_packet.h"
#include "util/log.h"

int
main(int argc, char **argv)
{
	engine_init();
	engine_start();

	return 0;
}


