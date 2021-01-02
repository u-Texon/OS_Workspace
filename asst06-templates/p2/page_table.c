#define _POSIX_C_SOURCE 200112L
#include "page_table.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// The pointer to the base directory.
// You can safely assume that this is set before any address conversion is done.
static PageDirectory *_cr3 = NULL;

typedef struct {
    uint32_t accessCounter;
    int valid;

    // ----------------
    // Add more data that needs to be stored in the TLB here.
    // ----------------
} TLBEntry;

typedef struct {
    TLBEntry entries[TLB_SIZE];
    uint32_t accessCounter;
} TLB;

// This is reset to all 0.
static TLB _tlb;

void setPageDirectory(PageDirectory *directory)
{
    _cr3 = directory;
    flushTLB();
}

void flushTLB()
{
    memset(&_tlb, 0, sizeof(_tlb));
}

#define ENTRY_MASK (ENTRIES_PER_TABLE - 1)

// Returns the base address of the current frame
// (i.e., the address of the first byte in the frame)
static inline uint32_t _getVirtualBase(uint32_t address)
{
    return address & BASE_MASK;
}

// Returns the index in the page directory for the given address.
static inline uint32_t _getPageDirectoryIndex(uint32_t address)
{
    return address >> (OFFSET_BITS + BITS_PER_ENTRY);
}

// Returns the index in the second level page table for the given address.
static inline uint32_t _getPageTableIndex(uint32_t address)
{
    return (address >> OFFSET_BITS) & ENTRY_MASK;
}

// Returns the offset for the given address.
static inline uint32_t _getOffset(uint32_t address)
{
    return address & OFFSET_MASK;
}

int mapPage(uint32_t virtualBase, uint32_t physicalBase, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    if ((_getOffset(virtualBase) != 0) || (_getOffset(physicalBase) != 0)) {
        return -1;
    }

    assert(_cr3 != NULL);

    (void) accessMode;
    (void) privileges;

    // ----------------
    // Add a new page table entry.
    // If you need more space for your table data structure, you can use
    // posix_memalign() to get 4kB-Aligned blocks of memory
    // Remember to invalidate the TLB line for that page.
    // ----------------

    return 0;
}

#ifndef PAGE_FAULTS_H
// Called whenever an access to a non-present page occurs. The handler
// should not be called for access violations.
int handlePageFault(uint32_t virtualBase, uint32_t pte)
{
    /* Nothing to do here - you will implement this function in assignment 6.3! */
    (void) virtualBase;
    (void) pte;
    return -1;
}
#endif

int translatePageTable(uint32_t *address, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    assert(_cr3 != NULL);

    if (address == NULL) {
        return -1;
    }

    (void) accessMode;
    (void) privileges;

    // ----------------
    // Implement the address translation here.
    // ----------------

    return -1;
}

void invalidateTLBEntry(uint32_t virtualBase)
{
    assert(_getOffset(virtualBase) == 0);

    // ----------------
    // Implement the TLB invalidation here.
    // ----------------
}

static void _addToTLBAt(int index, uint32_t virtualBase, uint32_t pte)
{
    _tlb.entries[index].accessCounter = _tlb.accessCounter;
    _tlb.entries[index].valid = 1;
    _tlb.accessCounter++;

    (void) virtualBase;
    (void) pte;

    // ----------------
    // Add the entry to your TLB.
    // ----------------
}

int addToTLB(uint32_t virtualBase, uint32_t pte)
{
    if (_getOffset(virtualBase) != 0) {
        return -1;
    }

    int oldestEntry = 0;
    for (int i = 1; i < TLB_SIZE; i++) {
        if (!_tlb.entries[i].valid) {
            oldestEntry = i;
            break;
        }

        if (_tlb.entries[i].accessCounter < _tlb.entries[oldestEntry].accessCounter) {
            oldestEntry = i;
        }
    }

    _addToTLBAt(oldestEntry, virtualBase, pte);

    return 0;
}

int translateTLB(uint32_t *address, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    if (address == NULL) {
        return -1;
    }

    (void) accessMode;
    (void) privileges;

    // ----------------
    // Implement the TLB translation here.
    // ----------------

    return -1;
}
