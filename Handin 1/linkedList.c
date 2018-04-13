#include "linkedList.h"
#include <stdlib.h>
#define LIST_ADD_OK 0
#define LIST_ADD_ERROR 1
#define LIST_REMOVE_REMOVED 0
#define LIST_REMOVE_NOT_FOUND 1
#define LIST_REMOVE_NULLPTR 2

int list_add(List *list, const void *element)
{
	Node *new_node = (Node*)malloc(sizeof *new_node);
	if (new_node == NULL)
		return LIST_ADD_ERROR;
	
	new_node->element = element;
	new_node->next = list->head;
	list->head = new_node;
	list->count++;
	return LIST_ADD_OK;
}
int list_remove(List *list, const void *element)
{
	if (!list)
		return LIST_REMOVE_NULLPTR;
	if (!(list->head))
		return LIST_REMOVE_NOT_FOUND; //list empty

	Node *first = list->head;
	if (first->element == element)
	{
		//Special case: element is first in chain
		list->head = first->next;
		free(first);
		list->count--;
		return LIST_REMOVE_REMOVED;
	}

	while (first->next != NULL && first->next->element != element && (first = first->next)); //spin till the node before the element we're looking for, or out of bounds
	
	if (first->next != NULL && first->next->element == element)
	{
		Node *del = first->next;
		first->next = first->next->next;
		free(del);
		list->count--;
		return LIST_REMOVE_REMOVED;
	}
	else
	{
		//not in chain
		return LIST_REMOVE_NOT_FOUND;
	}
}
size_t list_count(List *list)
{
	if (list)
		return list->count;
	return 0;
}
void *list_get(List *list, int32_t index)
{
	if (list)
	{
		Node *head = list->head;
		int32_t length = list_count(list);
		if (index >= 0 && length > 0 && index < length)
		{
			length -= index + 1;
			while (length != 0)
			{
				if (!head)
					return 0;
				head = head->next;
				length--;
			}
			if (!head)
				return 0;
			return head->element;
		}
	}
	return 0;
}

List *new_list()
{
	List *list = (List*)malloc(sizeof *list);
	if (list)
	{
		list->head = 0;
		list->count = 0;
	}
	return list;
}
int32_t list_indexof(List *list, const void *element)
{
	if (!list)
		return -1;
	int32_t c = 1;
	Node *first = list->head;
	while (first && first->element != element && (first = first->next))
		c++;
	return list->count - c;
}

