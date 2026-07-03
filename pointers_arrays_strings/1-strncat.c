#include "main.h"

/**
 * _strncat - concatenates two strings, using at most n bytes from src
 * @dest: the destination string
 * @src: the string to append to dest
 * @n: the maximum number of bytes to use from src
 *
 * Return: a pointer to the resulting string dest
 */
char *_strncat(char *dest, char *src, int n)
{
	int i;
	int j;

	i = 0;
	j = 0;

	while (dest[i] != '\0')
	{
		i++;
	}

	while (src[j] != '\0' && j < n)
	{
		dest[i + j] = src[j];
		j++;
	}

	dest[i + j] = '\0';
	return (dest);
}
