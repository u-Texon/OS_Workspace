#include "parseint.h"

/*
 * Returns the value of c or -1 on error
 */
int parseDecimalChar(char c)
{
    if (c > '9' || c < '0')
    {
        return -1;
    }
    // subtract the ASCI value (48) from c to get the corresponding int value
    return c - 48;
}

int getLength(char *string)
{
    int size = 0;
    for (int i = 0; string[i] != '\0'; i++)
    {
        size++;
    }
    return size;
}

int calcInASystem(int system, char *string)
{
    if (*string == '\0')
    {
        return -1;
    }

    int newInt = 0;
    int factor = 1;
    int size = getLength(string);

    for (int i = size - 1; string[i] != '\0'; i--)
    {
        int parsedInt = parseDecimalChar(string[i]);
        if (parsedInt == -1)
        {
            return -1;
        }
        newInt += parsedInt * factor;
        factor = factor * system;
    }
    return newInt;
}

/*
 * Parses a non-negative integer, interpreted as octal when starting with 0,
 * decimal otherwise. Returns -1 on error.
 */
int parseInt(char *string)
{
    if (string[0] == '0')
    {
        return calcInASystem(8, string);
    }
    else
    {
        return calcInASystem(10, string);
    }
}