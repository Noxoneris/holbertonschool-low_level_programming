#include "lists.h"

/**
 * list_len - count  the number of elements in a list_t list
 * @h: pointer to the first node of the list
 *
 * Return: the number of elements
 */
size_t list_len(const list_t *h)
{
	const list_t *tmp;
	size_t count;

	tmp = h;
	count = 0;
	while (tmp != NULL)
	{
		count++;
		tmp = tmp->next;
	}
	return (count);
}
