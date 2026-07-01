#include <stdio.h>

int main(void)
{
	int choice = -1;
	int a = 0;
	int b = 0;
	int result = 0;

	printf("Simple Calculator\n");

	while (choice != 0)
	{
		printf("1) Add\n");
		printf("2) Subtract\n");
		printf("3) Multiply\n");
		printf("4) Divide\n");
		printf("0) Quit\n");
		printf("Choice: ");
		scanf("%d", &choice);
		if (choice == 1)
		{
			printf("A: ");
			scanf("%d", &a);
			printf("B: ");
			scanf("%d", &b);
			result = a + b;
			printf("Result: %d\n", result);
		}
		else if (choice == 2)
		{
			printf("A: ");
			scanf("%d", &a);
			printf("B: ");
			scanf("%d", &b);
			result = a - b;
			printf("Result: %d\n", result);
		}
		else if (choice == 3)
                {
                        printf("A: ");
                        scanf("%d", &a);
                        printf("B: ");
                        scanf("%d", &b);
                        result = a * b;
                        printf("Result: %d\n", result);
                }
		else if (choice == 4)
                {
                        printf("A: ");
                        scanf("%d", &a);
                        printf("B: ");
                        scanf("%d", &b);
			if (b == 0)
				printf("Error: division by zero\n");
			else
			{
				result = a / b;
				printf("Result: %d\n", result);
			}
                }
		else if (choice < 1 || choice > 4)
			printf("Invalid choice\n");
	}
	printf("Bye!\n");
	return (0);
}
