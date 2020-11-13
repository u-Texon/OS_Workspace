#include "parseint.h"

/*
 * Returns the value of c or -1 on error
 */
int parseDecimalChar(char c)
{
    if  (c > '9' || c < '0')
    {
        return -1;
    }
    // subtract the ASCI value (48) from c to get the corresponding int value
    return c - 48;
}

/*
 * Parses a non-negative integer, interpreted as octal when starting with 0,
 * decimal otherwise. Returns -1 on error.
 */
int parseInt(char *string)
{
    (void)string;

    return -1;
}