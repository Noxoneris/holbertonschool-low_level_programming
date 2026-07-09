#include "main.h"
#include <stdlib.h>

/**
 * fill_zero - sets each byte of a buffer to zero
 * @buffer: the buffer to fill
 * @total: total number of bytes
 *
 * Return: Nothing.
 */
void fill_zero(char *buffer, unsigned int total)
{
	unsigned int i;

	for (i = 0; i < total; i++)
		buffer[i] = 0;
}

/**
 * _calloc - allocates memory for an array, initialized to zero
 * @nmemb: number of elements
 * @size: size of each element
 *
 * Return: pointer to allocated memory, or NULL on failure
 */
void *_calloc(unsigned int nmemb, unsigned int size)
{
	void *result;

	if (nmemb == 0 || size == 0)
		return (NULL);

	result = malloc(nmemb * size);
	if (result == NULL)
		return (NULL);

	fill_zero(result, nmemb * size);
	return (result);
}
