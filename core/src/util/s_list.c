#include "s_list.h"
#include "log.h"

void
s_list_add(struct s_list_node **head, void *data)
{
	struct s_list_node *new_node = malloc(sizeof(struct s_list_node));
		
	new_node->data = data;	
	new_node->next = *head;

	*head = new_node;
}

void
s_list_remove(struct s_list_node **head, const void *data)
{

	struct s_list_node *cur = *head;
	struct s_list_node *prev = cur;
	
	if (*head == NULL) return;

	while (cur != NULL)
	{
		if (cur->data == data)
		{
			prev->next = cur->next;
			if (cur == *head)
			{
				*head = cur->next;
			}

			free(cur);
			return;
		}

		prev = cur;
		cur = cur->next;
	}
}

bool
s_list_contains(struct s_list_node *head, void *data)
{
	const struct s_list_node *temp = head;

	while (temp != NULL)
	{
		if (data == temp->data)
		{
			return true;
		}
		temp = temp->next;
	}

	return false;
}

void
s_list_clear(struct s_list_node **head)
{
	struct s_list_node *cur = *head;
	struct s_list_node *n = NULL;

	if (*head == NULL) return;

	while (cur != NULL)
	{
		n = cur->next;
		free(cur);
		cur = n;
	}	
	
	*head = NULL;
}

void
s_list_clear_data(struct s_list_node **head)
{
	struct s_list_node *cur = *head;
	struct s_list_node *n = NULL;

	if (*head == NULL) return;

	while (cur != NULL)
	{
		n = cur->next;
		free(cur);
		free(cur->data);
		cur = n;
	}	
	
	*head = NULL;
}

void
s_list_copy(struct s_list_node **dest, const struct s_list_node *src)
{
	const struct s_list_node *temp = src;

	while (temp != NULL)
	{
		s_list_add(dest, temp->data);	

		temp = temp->next;
	}
}

void
s_list_reverse(struct s_list_node **head)
{
	struct s_list_node* prev   = NULL;
    struct s_list_node* current = *head;
    struct s_list_node* next = NULL;
    while (current != NULL)
    {
        next  = current->next;  
        current->next = prev;   
        prev = current;
        current = next;
    }
    *head = prev;
}

void
s_list_print(const struct s_list_node *head)
{
	const struct s_list_node *cur = head;
		
	uint32_t count = 0;

	if (cur == NULL)
	{
		log_trace("s_list is empty...");	
		return;
	}

	while (cur != NULL)
	{
		count++;
		cur = cur->next;
	}
		
	log_trace("Item s_list count: %u", count);
}



