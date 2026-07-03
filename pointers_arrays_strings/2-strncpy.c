#include "main.h"

/**
 * _strncpy - copies a string, using at most n bytes from src
 * @dest: the destination string
 * @src: the source string
 * @n: the maximum number of bytes to copy
 *
 * Return: a pointer to the resulting string dest
 */
char *_strncpy(char *dest, char *src, int n)
{
	int i;

	i = 0;

	while (src[i] != '\0' && i < n)
	{
		dest[i] = src[i];
		i++;
	}

	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}

	return (dest);
}
