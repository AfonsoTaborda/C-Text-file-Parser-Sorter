#pragma once
#include <stdint.h>

typedef struct Node_t
{
	struct Node_t *next;
	const void *element;
} Node;
typedef struct List_t
{
	Node *head;
	size_t count;
} List;

List *new_list();
int list_add(List *list, const void *element);
int list_remove(List *list, const void *element);
size_t list_count(List *list);
void *list_get(List *list, int32_t index);
int32_t list_indexof(List *list, const void *element);

