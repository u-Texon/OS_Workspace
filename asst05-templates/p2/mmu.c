#include "mmu.h"
#include <stddef.h>

static SegmentTable *_table;

typedef struct
{
    //physical adress
    uint32_t physical;

    //virtuel adress
    uint32_t virtual;

    //counter to find the least used element of TLB
    size_t timesUsed;

} Translation;

typedef struct
{
    Translation translations[TLB_SIZE];
} TLB;

TLB _TLB;

void setSegmentTable(SegmentTable *newTable)
{
    _table = newTable;
    flushTLB();
}

int translateSegmentTable(uint32_t *address)
{
    uint32_t index = (*address >> OFFSET_BITS);
    //ofset should look like: 000|X XXXX XXXX XXXX XXXX XXXX XXXX XXXX
    uint32_t offset = (*address << SEGMENT_BITS) >> SEGMENT_BITS;

    Segment segment = _table->segments[index];
    if (segment.length == 0) {
        return -1;
    }
    if ((segment.base + offset >= segment.base) && (segment.base + offset <= segment.length))
    {
        *address = offset + segment.base;
        return 0;
    }

    return -1;
}

void flushTLB(void)
{
    for (size_t i = 0; i < TLB_SIZE; i++)
    {
        Translation translation;
        translation.physical = -1;
        translation.virtual = -1;
        translation.timesUsed = 0;
        _TLB.translations[i] = translation;
    }
}

/*
 *  ths method searches for the next free index of TLB and returns it.
 *  if every element of TLB is full it returns the index the least used element of TLB.
 */
size_t findTLB()
{
    for (size_t i = 0; i < TLB_SIZE; i++)
    {
        if (_TLB.translations[i].timesUsed == 0)
        {
            _TLB.translations[i].timesUsed++;
            return i;
        }
    }
    size_t leastUsed = _TLB.translations[0].timesUsed;
    size_t x = 0;
    for (size_t i = 1; i < TLB_SIZE; i++)
    {
        if (leastUsed > _TLB.translations[i].timesUsed)
        {
            leastUsed = _TLB.translations[i].timesUsed;
            x = i;
        }
    }
    return x;
}

void addToTLB(uint32_t virtual, uint32_t physical)
{
    size_t i = findTLB();
    _TLB.translations[i].physical = physical;
    _TLB.translations[i].virtual = virtual;
    _TLB.translations[i].timesUsed++;
}

int translateTLB(uint32_t *address)
{
    for (size_t i = 0; i < TLB_SIZE; i++)
    {
        if (_TLB.translations[i].virtual == *address)
        {
            *address = _TLB.translations[i].physical;
            _TLB.translations[i].timesUsed++;
            return 0;
        }
    }

    return -1;
}