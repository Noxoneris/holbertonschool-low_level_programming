#include "main.h"
#include <stdlib.h>
#include <string.h>

/**
 * fill_concat - copies s1 and n bytes of s2 into a new buffer
 * @dest: destination buffer
 * @s1: first string
 * @s2: second string
 * @len1: length of s1 to copy
 * @len2: length of s2 to copy
 *
 * Return: Nothing.
 */
void fill_concat(char *dest, char *s1, char *s2,
		unsigned int len1, unsigned int len2)
{
	unsigned int i;

	for (i = 0; i < len1; i++)
		dest[i] = s1[i];
	for (i = 0; i < len2; i++)
		dest[len1 + i] = s2[i];
	dest[len1 + len2] = '\0';
}

/**
 * string_nconcat - concatenates two strings
 * @s1: first string
 * @s2: second string
 * @n: number of bytes of s2 to use
 *
 * Return: pointer to newly allocated concatenated string, or NULL on failure
 */
char *string_nconcat(char *s1, char *s2, unsigned int n)
{
	unsigned int len1, len2;
	char *result;

	len1 = (s1 == NULL) ? 0 : strlen(s1);
	if (s2 == NULL)
		len2 = 0;
	else if (n >= strlen(s2))
		len2 = strlen(s2);
	else
		len2 = n;

	result = malloc(len1 + len2 + 1);
	if (result == NULL)
		return (NULL);

	fill_concat(result, s1, s2, len1, len2);
	return (result);
}
