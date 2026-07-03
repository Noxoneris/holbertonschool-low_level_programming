#include "main.h"

/**
 * _strcmp - compares two strings
 * @s1: the first string
 * @s2: the second string
 *
 * Return: the difference between the first differing characters,
 * or 0 if the strings are identical
 */
int _strcmp(char *s1, char *s2)
{
	int i;

	i = 0;

	while (s1[i] == s2[i] && s1[i] != '\0')
	{
		i++;
	}

	return (s1[i] - s2[i]);
}
