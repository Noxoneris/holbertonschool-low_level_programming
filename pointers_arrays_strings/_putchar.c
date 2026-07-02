#include "main.h"

/**
 * _putchar - writes a character to stdout
 * @c: the character to print
 *
 * Return: 1 on success, or -1 on error
 */
int _putchar(char c)
{
	return (write(1, &c, 1));
}
