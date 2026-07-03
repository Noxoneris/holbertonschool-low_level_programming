#include "main.h"

/**
 * _memset - fills memory with a constant byte
 * @s: the memory area to fill
 * @b: the byte to fill the memory with
 * @n: the number of bytes to fill
 *
 * Return: a pointer to the memory area s
 */
char *_memset(char *s, char b, unsigned int n)
{
	unsigned int i;

	i = 0;

	while (i < n)
	{
		s[i] = b;
		i++;
	}

	return (s);
}
