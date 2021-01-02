#include "insert.h"

#include <string.h>
#include <stdlib.h>

/*
 * Reminder: insert() function from the tutorial that does not manage capacity.
 */
void insert_tut(int *a, size_t *length, int z)
{
	int *p = a;
	int *end = a + *length;
	while ((p < end) && (*p < z))
	{
		++p;
	}
	memmove(p + 1, p, (end - p) * sizeof(int));
	*p = z;
	(*length)++;
}

/*
 * New insert() function that reallocates if the array is full.
 */
void insert(int **array, size_t *length, size_t *capacity, int z)
{
	if (*length == *capacity)
	{	
		*capacity += 10;
		int *newArr = malloc(sizeof(int) * *capacity);
		memmove(newArr, *array, sizeof(int) * (*capacity - 10));
		free(*array);
		*array = newArr;
	}
		insert_tut(*array, length, z);
}
