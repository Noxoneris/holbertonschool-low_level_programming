#include "main.h"

/**
 * puts_half - prints the second half of a string, followed by a new line
 * @str: the string to print half of
 *
 * Return: nothing
 */
void puts_half(char *str)
{
	int len;
	int n;
	int i;

	len = 0;
	while (str[len] != '\0')
	{
		len++;
	}

	n = (len + 1) / 2;

	i = n;
	while (str[i] != '\0')
	{
		_putchar(str[i]);
		i++;
	}
	_putchar('\n');
}
