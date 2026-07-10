#include <stdlib.h>
#include "dog.h"

/**
 * my_strlen - calculates the length of a string
 * @s: the string
 *
 * Return: length of the string
 */
unsigned int my_strlen(char *s)
{
	unsigned int len;

	len = 0;
	while (s[len] != '\0')
		len++;
	return (len);
}

/**
 * my_strdup - duplicates a string into newly allocated memory
 * @s: the string to duplicate
 *
 * Return: pointer to the duplicated string, or NULL on failure
 */
char *my_strdup(char *s)
{
	char *copy;
	unsigned int i, len;

	len = my_strlen(s);
	copy = malloc(len + 1);
	if (copy == NULL)
		return (NULL);
	for (i = 0; i < len; i++)
		copy[i] = s[i];
	copy[len] = '\0';
	return (copy);
}

/**
 * new_dog - creates a new dog
 * @name: name of the dog
 * @age: age of the dog
 * @owner: owner of the dog
 *
 * Return: pointer to the new dog, or NULL on failure
 */
dog_t *new_dog(char *name, float age, char *owner)
{
	dog_t *d;

	d = malloc(sizeof(dog_t));
	if (d == NULL)
		return (NULL);

	d->name = my_strdup(name);
	if (d->name == NULL)
	{
		free(d);
		return (NULL);
	}

	d->owner = my_strdup(owner);
	if (d->owner == NULL)
	{
		free(d->name);
		free(d);
		return (NULL);
	}

	d->age = age;
	return (d);
}
