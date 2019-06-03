#include <math.h>
#include <time.h>
#include "network/server.h"
#include "database/db.h"
#include "util/log.h"
#include "network/server_funcs.h"
#include "data/data_spell.h"

static bool running;

void 
loop_server(void)
{
	running = true;

	struct timespec sleep_time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 1000000; //1ms
	
	id_pools_init();
	data_spell_init();
	data_hero_init();
	server_init();
	db_init();

	server_manager_init();

	log_info("Server initialized.");

	double game_period = (1000.0 / TICKS_PER_SECOND);
	
	long long int now = get_time_ms();
	long long int last_game = get_time_ms();

	while (running)
	{
		now = get_time_ms();
		
		server_read();
	
		if (now - last_game > game_period)
		{
			server_process();
			server_manager_update();
			server_write_packets();
			last_game += game_period;
		}


		nanosleep(&sleep_time, NULL);
	}
	
	id_pools_destroy();
	server_manager_destroy();
	db_destroy();
}

int
main(int argc, char **argv)
{
	loop_server();

	return 1;
}


