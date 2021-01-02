#include "hash.h"

/*
 * Returns a hash value for the given string.
 */
uint64_t hashString(char *c)
{
	int hn = 5381;
	int sum = 0;

	int i = 0;
	while (c[i] != '\0')
	{
		hn = hn * 33 + c[i];
		sum = sum + hn;
		i++;
	}
	return sum;
}

/*
 * Inserts a key-value pair into the hash table.
 */
int insert(hashtable *table, char *key, int value)
{
	int size = table->capacity;
	int position = hashString(key) % size;
	if (table->elements[position].value != 0)
	{
		if (table->elements->key == key)
		{
			table->elements[position].value = value;
			return 1;
		}
	}
	else
	{
		table->elements[position].key = key;
		table->elements[position].value = value;
		return 1;
	}

	int startP = position++;
	while (position != startP)
	{
		if (table->elements[position].value == 0)
		{
			table->elements[position].key = key;
			table->elements[position].value = value;
			return 1;
		}
		else if (table->elements[position].key == key)
		{
			table->elements[position].value = value;
			return 1;
		}
		position++;
		if (position == size)
		{
			position = 0;
		}
	}
	return 0;
}

/*
 * Retrieves the value for a given key.
 */
int find(hashtable *table, char *key, int *value)
{
	int size = table->capacity;
	int position = hashString(key) % size;

	if (table->elements[position].value == 0)
	{
		return 0;
	}
	else if (table->elements[position].key == key)
	{
		*value = table->elements[position].value;
		return 1;
	}
	int startPos = position++;

	while (startPos != position)
	{
		if (table->elements[position].value == 0)
		{
			return 0;
		}
		else if (table->elements[position].key == key)
		{
			*value = table->elements[position].value;
			return 1;
		}
		position++;
		if (position == size)
		{
			position = 0;
		}
	}

	return 0;
}