#include "strings.h"

/**
 * Returns the length of string s without counting the terminating null byte.
 */
size_t stringlength(const char *s)
{
    size_t counter = 0;
    while (s[counter] != '\0')
    {
        counter++;
    }
    return counter;
}

/**
 * Returns a new null byte terminated string that is the result of appending string s2 to s1.
 * The caller is responsible to free the returned string.
 * Returns NULL on any error.
 */
char *stringconcat(const char *s1, const char *s2)
{
    size_t l1 = stringlength(s1);
    size_t l2 = stringlength(s2);

    size_t stringLength = l1 + l2;

    char *p = malloc(sizeof(char) * stringLength);
    if (p == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < l1; i++)
    {
        p[i] = s1[i];
    }
    for (size_t i = l1; i < stringLength; i++)
    {
        p[i] = s2[i - l1];
    }
    return p;
}

/**
 * Splits string toSplit at every occurrence of the delimiter.
 * Returns a null-terminated array with pointers to the null-terminated
 * substrings. The caller should free the result by calling stringsplit_free().
 * Returns NULL on any error.
 */
char **stringsplit(const char *toSplit, char delimiter)
{
    size_t length = stringlength(toSplit);
    char **string = malloc(sizeof(char) * length);
    size_t subNum = 1;
    for (size_t i = 0; i < length; i++)
    {
        if (toSplit[i] == delimiter)
        {
            subNum++;
        }
    }

    size_t index = 0;
    size_t i = 0;
    while (index < subNum)
    {
        size_t j = 0;
        char *p = malloc(sizeof(char) * length);
        while (toSplit[i] != delimiter && i < length)
        {
            p[j] = toSplit[i];
            i++;
            j++;
        }
        i++;
        string[index] = p;
        index++;
    }
    string[index] = NULL;

    return string;
}

/**
 * Frees the result from a stringsplit() call.
 */
void stringsplit_free(char **parts)
{
    free(parts);
}
