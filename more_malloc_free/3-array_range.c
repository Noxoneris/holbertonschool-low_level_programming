#include "main.h"
#include <stdlib.h>

/**
 * fill_range - fills an array with values from min to max
 * @result: the array to fill
 * @min: the starting value (included)
 * @max: the ending value (included)
 *
 * Return: Nothing.
 */
void fill_range(int *result, int min, int max)
{
	int i;

	for (i = 0; i < max - min + 1; i++)
		result[i] = min + i;
}

/**
 * array_range - creates an array of integers from min to max
 * @min: the starting value (included)
 * @max: the ending value (included)
 *
 * Return: pointer to the newly created array, or NULL on failure
 */
int *array_range(int min, int max)
{
	int *result;

	if (min > max)
		return (NULL);

	result = malloc((max - min + 1) * sizeof(int));
	if (result == NULL)
		return (NULL);

	fill_range(result, min, max);
	return (result);
}
