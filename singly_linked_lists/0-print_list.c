#include <stdio.h>
#include "lists.h"

/**
 * print_list - print all the elements of a list_t list
 * @h: pointer to the first node of the list (head)
 *
 * Return: the number of nodes
 */
size_t print_list(const list_t *h)
{
	const list_t *tmp;
	size_t count;

	tmp = h;
	count = 0;
	while (tmp != NULL)
	{
		if (tmp->str == NULL)
			printf("[0] (nil)\n");
		else
			printf("[%u] %s\n", tmp->len, tmp->str);
		count++;
		tmp = tmp->next;
	}
	return (count);
}
