#ifndef GAME__
#define GAME__


#include "world.h"
#include "command.h"

struct
game
{
	
	struct command commands[MAX_COMMANDS];
	//shouldnt be pointers??? commands??
	struct command command_history[MAX_COMMANDS_HISTORY]; // Little over 15 minutes of history
	uint16_t command_count_history;

	struct world world;

};

struct game*
game_init(void);

void
game_update(struct game *game);

struct command*
game_command_init(struct game *game);

void
game_command_dispose(struct game *game, struct command *command);

void
game_destroy(struct game *game);

#endif





