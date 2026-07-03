#include "main.h"

/**
 * _atoi - converts a string to an integer
 * @s: the string to convert
 *
 * Return: the converted integer, or 0 if no digits found
 */
int _atoi(char *s)
{
	int i;
	int sign;
	int nombre;

	i = 0;
	sign = 1;
	nombre = 0;

	while ((s[i] == '-' || s[i] == '+' || (s[i] < '0' ||
		 s[i] > '9')) && s[i] != '\0')
	{
		if (s[i] == '-')
		{
			sign = sign * -1;
		}
		i++;
	}

	while (s[i] >= '0' && s[i] <= '9')
	{
		nombre = nombre * 10 + (s[i] - '0');
		i++;
	}


	if (sign == 1)
		return (-nombre);
	return (nombre);
}
