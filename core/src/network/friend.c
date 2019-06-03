#include <string.h>
#include <limits.h>
#include "friend.h"
#include "../util/log.h"

struct friend
friend_init(uint32_t id, char *username, bool online)
{
	struct friend fr;

	fr.id = id;
	strcpy(fr.username, username);
	fr.online = online;
	fr.name_color.c = COLOR_WHITE;

	return fr;
}

struct friend_list
friend_list_init(void)
{
	struct friend fr = friend_init(UINT_MAX, "", false);

	struct friend_list list;
	list.friend_count = 0;

	for (int i = 0; i < MAX_FRIENDS; i++)
	{
		list.friends[i] = fr;
	}

	return list;
}

void
friend_list_add(struct friend_list *list, struct friend fr)
{	
	for (int i = 0; i < MAX_FRIENDS; i++)
	{
		if (list->friends[i].id == UINT_MAX)
		{
			list->friends[i] = fr;
			break;
		}
	}
}

void
friend_list_remove(struct friend_list *list, struct friend fr)
{	
	for (int i = 0; i < MAX_FRIENDS; i++)
	{
		if (list->friends[i].id == fr.id)
		{
			list->friends[i] = friend_init(UINT_MAX, "", false);
			break;
		}
	}
}

