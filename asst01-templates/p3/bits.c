#include <stdint.h>

#include "bits.h"

/*
 * Returns the n'th bit of the array A.
 */
int getN(uint64_t *A, size_t n)
{
    int arrayNum = n / 64;
    int rest = n % 64;
    return (A[arrayNum] >> (n - rest)) & 1;
}

/*
 * Sets the n'th bit of the array A.
 */
void setN(uint64_t *A, size_t n)
{
    int arrayNum = n / 64;
    int rest = n % 64;
    A[arrayNum] = (1 << (n - rest)) | A[arrayNum];
}

/*
 * Clears the n'th bit of the array A.
 */
void clrN(uint64_t *A, size_t n)
{
    int arrayNum = n / 64;
    int rest = n % 64;
    A[arrayNum] = A[arrayNum] << (n - rest) & 0;
}

/*
 * Rotates the integer i n bits to the right.
 */
uint64_t rot(uint64_t i, int n)
{
    //idk
    return -1;
}
