#include "main.h"
#include <stdlib.h>

/**
 * free_partial_grid - frees a partially allocated grid
 * @grid: the grid to free
 * @allocated_rows: the number of rows already allocated
 *
 * Return: nothing
 */
void free_partial_grid(int **grid, int allocated_rows)
{
	while (allocated_rows > 0)
	{
		allocated_rows--;
		free(grid[allocated_rows]);
	}
	free(grid);
}

/**
 * alloc_grid - returns a pointer to a 2 dimensional array of integers
 * @width: the width of the grid
 * @height: the height of the grid
 *
 * Return: a pointer to the newly allocated grid, or NULL on failure
 */
int **alloc_grid(int width, int height)
{
	int **grid;
	int h;
	int w;

	if (width <= 0 || height <= 0)
	{
		return (NULL);
	}

	grid = malloc(sizeof(int *) * height);
	if (grid == NULL)
	{
		return (NULL);
	}

	h = 0;
	while (h < height)
	{
		grid[h] = malloc(sizeof(int) * width);
		if (grid[h] == NULL)
		{
			free_partial_grid(grid, h);
			return (NULL);
		}

		w = 0;
		while (w < width)
		{
			grid[h][w] = 0;
			w++;
		}

		h++;
	}

	return (grid);
}
