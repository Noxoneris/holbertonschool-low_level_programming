#include <stdarg.h>
#include <stdlib.h>
#include "variadic_functions.h"

/**
 * print_char - prints a char
 * @args: va_list containing the char
 */
void print_char(va_list args)
{
	printf("%c", va_arg(args, int));
}

/**
 * print_int - prints an int
 * @args: va_list containing the int
 */
void print_int(va_list args)
{
	printf("%d", va_arg(args, int));
}

/**
 * print_float - prints a float
 * @args: va_list containing the float
 */
void print_float(va_list args)
{
	printf("%f", va_arg(args, double));
}

/**
 * print_string - prints a string
 * @args: va_list containing the string
 */
void print_string(va_list args)
{
	char *str;

	str = va_arg(args, char *);

	if (str == NULL)
	{
		str = "(nil)";
	}

	printf("%s", str);
}

/**
 * print_all - prints anything
 * @format: list of types of arguments passed to the function
 * @...: the arguments to print
 *
 * Return: Nothing.
 */
void print_all(const char * const format, ...)
{
	va_list args;
	int i;
	char *separator;
	char types[] = {'c', 'i', 'f', 's'};
	void (*funcs[])(va_list) = {print_char, print_int, print_float, print_string};

	va_start(args, format);
	separator = "";
	i = 0;

	while (format != NULL && format[i] != '\0')
	{
		int j = 0;

		while (j < 4)
		{
			if (format[i] == types[j])
			{
				printf("%s", separator);
				funcs[j](args);
				separator = ", ";
			}
			j++;
		}
		i++;
	}

	printf("\n");

	va_end(args);
}
