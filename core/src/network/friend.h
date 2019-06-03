#ifndef FRIEND__
#define FRIEND__

#include <inttypes.h>
#include <stdbool.h>
#include "../util/game_values.h"
#include "../util/game_util.h"

struct
friend
{
	uint32_t id;

	char username[MAX_USERNAME_LEN+1];
	union color name_color;
	bool online;
};


struct
friend_list
{
	struct friend friends[MAX_FRIENDS];
	uint16_t friend_count;
};

struct friend
friend_init(uint32_t id, char *username, bool online);

struct friend_list
friend_list_init(void);

void
friend_list_add(struct friend_list *list, struct friend fr);

void
friend_list_remove(struct friend_list *list, struct friend fr);

#endif



