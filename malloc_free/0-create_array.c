#include "main.h"
#include <stdlib.h>

/**
 * create_array - creates an array of chars, initialized with a specific char
 * @size: the number of elements in the array
 * @c: the character to initialize each element with
 *
 * Return: a pointer to the array, or NULL if size is 0 or if it fails
 */
char *create_array(unsigned int size, char c)
{
	char *array;
	unsigned int i;

	if (size == 0)
	{
		return (NULL);
	}

	array = malloc(sizeof(char) * size);

	if (array == NULL)
	{
		return (NULL);
	}

	i = 0;
	while (i < size)
	{
		array[i] = c;
		i++;
	}

	return (array);
}
