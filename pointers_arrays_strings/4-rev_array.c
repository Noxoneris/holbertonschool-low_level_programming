#include "main.h"

/**
 * reverse_array - reverses the content of an array of integers
 * @a: the array to reverse
 * @n: the number of elements in the array
 *
 * Return: nothing
 */
void reverse_array(int *a, int n)
{
	int start;
	int end;
	int tmp;

	start = 0;
	end = n - 1;

	while (start < end)
	{
		tmp = a[start];
		a[start] = a[end];
		a[end] = tmp;
		start++;
		end--;
	}
}
