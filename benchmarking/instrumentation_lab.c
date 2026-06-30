#include <stdio.h>
#include <time.h>

#define DATASET_SIZE 50000
#define SEED_VALUE 42u

static int dataset[DATASET_SIZE];

static unsigned int next_value(unsigned int *state)
{
	*state = (*state * 1103515245u) + 12345u;
	return *state;
}

static void build_dataset(void)
{
	unsigned int state;
	int i;

	state = SEED_VALUE;

	for (i = 0; i < DATASET_SIZE; i++)
		dataset[i] = (int)(next_value(&state) % 100000);
}

static void process_dataset(void)
{
	int i;
	int v;

	for (i = 0; i < DATASET_SIZE; i++)
	{
		v = dataset[i];
		v = (v * 3) + (v / 7) - (v % 11);
		if (v < 0)
			v = -v;
		dataset[i] = v;
	}
}

static unsigned long reduce_checksum(void)
{
	unsigned long sum;
	int i;

	sum = 0;
	for (i = 0; i < DATASET_SIZE; i++)
		sum = (sum * 131ul) + (unsigned long)dataset[i];

	return sum;
}

int main(void)
{
	unsigned long checksum;
	clock_t total_start, total_end;
	clock_t build_start, build_end;
	clock_t process_start, process_end;
	clock_t reduce_start, reduce_end;
	double total_elapsed, build_elapsed, process_elapsed, reduce_elapsed;

	total_start = clock();

	build_start = clock();
	build_dataset();
	build_end = clock();

	process_start = clock();
	process_dataset();
	process_end = clock();

	reduce_start = clock();
	checksum = reduce_checksum();
	reduce_end = clock();

	total_end = clock();

	total_elapsed = (double)(total_end - total_start) / CLOCKS_PER_SEC;
	build_elapsed = (double)(build_end - build_start) / CLOCKS_PER_SEC;
	process_elapsed = (double)(process_end - process_start) / CLOCKS_PER_SEC;
	reduce_elapsed = (double)(reduce_end - reduce_start) / CLOCKS_PER_SEC;

	if (checksum == 0ul)
		printf("impossible\n");

	printf("TOTAL seconds: %.6f\n", total_elapsed);
	printf("BUILD_DATA seconds: %.6f\n", build_elapsed);
	printf("PROCESS seconds: %.6f\n", process_elapsed);
	printf("REDUCE seconds: %.6f\n", reduce_elapsed);

	return 0;
}
