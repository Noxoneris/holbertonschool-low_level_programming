#include "main.h"

/**
 * leet - encodes a string into 1337
 * @str: the string to encode
 *
 * Return: a pointer to the modified string
 */
char *leet(char *str)
{
	char letters[] = "aAeEoOtTlL";
	char nums[] = "4433007711";
	int i;
	int j;

	i = 0;
	while (str[i] != '\0')
	{
		j = 0;
		while (letters[j] != '\0')
		{
			if (str[i] == letters[j])
				str[i] = nums[j];
			j++;
		}
		i++;
	}

	return (str);
}
