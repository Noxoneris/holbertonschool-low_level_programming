#include "main.h"
#include <stdlib.h>

/**
 * _strdup - returns a pointer to a newly allocated copy of a string
 * @str: the string to duplicate
 *
 * Return: a pointer to the duplicated string, or NULL if str is NULL
 * or if insufficient memory was available
 */
char *_strdup(char *str)
{
	char *dup;
	unsigned int len;
	unsigned int i;

	if (str == NULL)
	{
		return (NULL);
	}

	i = 0;
	while (str[i] != '\0')
	{
		i++;
	}
	len = i;

	dup = malloc(sizeof(char) * (len + 1));

	if (dup == NULL)
	{
		return (NULL);
	}

	i = 0;
	while (i < len)
	{
		dup[i] = str[i];
		i++;
	}
	dup[i] = '\0';

	return (dup);
}
