#include "main.h"
#include <stdlib.h>

/**
 * free_grid - frees a 2 dimensional grid previously created
 * @grid: the address of the two dimensional grid
 * @height: height of the grid (number of rows)
 *
 * Return: Nothing.
 */
void free_grid(int **grid, int height)
{
	int h;

	h = 0;
	while (h < height)
	{
		free(grid[h]);
		h++;
	}
	free(grid);
}
