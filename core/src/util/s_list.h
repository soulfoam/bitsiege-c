#ifndef S_LIST__
#define S_LIST__

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

struct
s_list_node
{
	void *data;
	struct s_list_node *next;
};

void
s_list_add(struct s_list_node **head, void *data);

void
s_list_remove(struct s_list_node **head, const void *data);

void
s_list_clear(struct s_list_node **head);

void
s_list_clear_data(struct s_list_node **head);

bool
s_list_contains(struct s_list_node *head, void *data);

void
s_list_copy(struct s_list_node **dest, const struct s_list_node *src);

void
s_list_reverse(struct s_list_node **head);

void
s_list_print(const struct s_list_node *head);

#endif

