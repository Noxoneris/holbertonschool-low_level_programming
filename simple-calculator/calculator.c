#include <stdio.h>

int main(void)
{
	int choice;

	printf("Simple Calculator\n");
	printf("1) Add\n");
	printf("2) Subtract\n");
	printf("3) Multiply\n");
	printf("4) Divide\n");
	printf("0) Quit\n");
	printf("Choice:  ");
	scanf("%d", &choice);
	if (choice == 0)
	{
		printf("Bye!\n");
	}

	 return (0);
}
