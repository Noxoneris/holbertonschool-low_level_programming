#include "main.h"

/**
 * print_triangle - prints a triangle followed by a new line
 * @size: size of the triangle
 *
 * Return: void
 */
void print_triangle(int size)
{

	int i;
	int j;

	if (size <= 0)
	{
		_putchar('\n');
	}
	else
	{
		i = 1;
		while (i <= size)
		{
			j = 0;
			while (j < size - i)
			{
				_putchar(' ');
				j++;
			}
			j = 0;
			while (j < i)
			{
				_putchar('#');
				j++;
			}
			_putchar('\n');
			i++;
		}
	}
}
