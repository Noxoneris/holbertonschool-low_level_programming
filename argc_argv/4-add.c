#include <stdio.h>
#include <stdlib.h>

/**
 * main - adds positive numbers
 * @argc: number of arguments
 * @argv: array of arguments
 *
 * Return: Always 0, or 1 if error.
 */
int main(int argc, char *argv[])
{
	int i;
	int j;
	int sum;

	if (argc == 1)
	{
		printf("0\n");
		return (0);
	}

	sum = 0;
	i = 1;
	while (i < argc)
	{
		j = 0;
		while (argv[i][j] != '\0')
		{
			if (argv[i][j] < '0' || argv[i][j] > '9')
			{
				printf("Error\n");
				return (1);
			}
			j = j + 1;
		}
		sum = sum + atoi(argv[i]);
		i = i + 1;
	}

	printf("%d\n", sum);

	return (0);
}
