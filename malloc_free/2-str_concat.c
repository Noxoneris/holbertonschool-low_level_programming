#include "main.h"
#include <stdlib.h>

/**
 * str_len - counts the length of a string
 * @s: the string to measure
 *
 * Return: the length of the string
 */
unsigned int str_len(char *s)
{
	unsigned int i;

	i = 0;
	while (s[i] != '\0')
	{
		i++;
	}
	return (i);
}

/**
 * str_concat - concatenates two strings
 * @s1: the first string
 * @s2: the second string
 *
 * Return: a pointer to the newly allocated concatenated string,
 * or NULL on failure
 */
char *str_concat(char *s1, char *s2)
{
	char *result;
	unsigned int len1;
	unsigned int len2;
	unsigned int i;
	unsigned int j;

	if (s1 == NULL)
	{
		s1 = "";
	}
	if (s2 == NULL)
	{
		s2 = "";
	}

	len1 = str_len(s1);
	len2 = str_len(s2);

	result = malloc(sizeof(char) * (len1 + len2 + 1));
	if (result == NULL)
	{
		return (NULL);
	}

	i = 0;
	while (i < len1)
	{
		result[i] = s1[i];
		i++;
	}

	j = 0;
	while (j < len2)
	{
		result[len1 + j] = s2[j];
		j++;
	}

	result[len1 + len2] = '\0';
	return (result);
}
