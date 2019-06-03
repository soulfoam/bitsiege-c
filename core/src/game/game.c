#include "game.h"
#include "../util/log.h"

struct game*
game_init(void)
{
	struct game *game = malloc(sizeof(struct game));

	for (int i = 0; i < MAX_COMMANDS; i++) game->commands[i].type = COMMAND_INVALID;
	for (int i = 0; i < MAX_COMMANDS_HISTORY; i++) game->command_history[i].type = COMMAND_INVALID;
	
	game->command_count_history = 0;

	game->world = world_init();
	
	log_info("Game initialized.");

	return game;
}

void
game_update(struct game *game)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		struct hero *h = &game->world.heroes[i];
		if (h->id_game == UINT_MAX) continue;

		*h->ss = *h;
	}

	for (int i = 0; i < MAX_COMMANDS; i++)
	{
		struct command *com = &game->commands[i];
		if (!com->processed && com->type != COMMAND_INVALID)
		{
			command_process(com, &game->world);
			game_command_dispose(game, com);
		}
	}

	world_update(&game->world);
}

struct command*
game_command_init(struct game *game)
{
	struct command *c = NULL;

	for (int i = 0; i < MAX_COMMANDS; i++)
	{
		c = &game->commands[i];
		if (c->type == COMMAND_INVALID) return c;
	}

	return c;
}

void
game_command_dispose(struct game *game, struct command *command)
{
	game->command_history[game->command_count_history] = *command;
	game->command_count_history++;

	command_destroy(command);
	command->type = COMMAND_INVALID;
}

void
game_destroy(struct game *game)
{
	world_destroy(&game->world);
	free(game);
}

