#include "testlib.h"
#include "page_table.h"
#include <stdio.h>

PageDirectory __attribute__((aligned(0x1000))) basePageDirectory;
// For addresses starting with 0x0000...
PageTable __attribute__((aligned(0x1000))) pageTable1;
// For addresses starting with 0x1000...
PageTable __attribute__((aligned(0x1000))) pageTable2;

#define WRONG_ADDRESS ((uint32_t) -1)

// This can help you debugging...
static void _dumpPageTable(PageTable *table)
{
    for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
        if ((table->entries[i] & PAGE_PRESENT_MASK) == 0) {
            continue;
        }

        char rw          = table->entries[i] & PAGE_READWRITE_MASK ? 'W' : ' ';
        char privileges  = table->entries[i] & PAGE_USERMODE_MASK ? 'U' : ' ';
        char accessed    = table->entries[i] & PAGE_ACCESSED_MASK ? 'A' : ' ';

        uint32_t address = table->entries[i] & PAGE_TABLE_ADDRESS_MASK;

        printf("   Entry %04d: Page base: 0x%08"PRIx32" %c %c %c\n", i,
                address, accessed, privileges, rw);
    }
}

static void _dumpPageDirectory(PageDirectory *dir)
{
    for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
        if ((dir->entries[i] & PAGE_PRESENT_MASK) == 0) {
            continue;
        }

        uint64_t address = dir->entries[i] & PAGE_DIRECTORY_ADDRESS_MASK;

        printf("%04d => Page table at 0x%016"PRIx64"\n", i, address);
        _dumpPageTable((PageTable*)intToPointer(address));
    }
}

// This is how an MMU would do the address conversion.
static uint32_t _doAddressConversion(uint32_t virtual, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    char rw          = (accessMode == ACCESS_WRITE) ? 'W' : 'R';
    char priv  = (privileges == KERNEL_MODE) ? 'K' : 'U';

    uint32_t address = virtual;

    if (translateTLB(&address, accessMode, privileges) == 0) {
        printf("Translated (using TLB): 0x%08"PRIx32" (%c,%c) --> 0x%08"PRIx32"\n",
                virtual, rw, priv, address);

        return address;       // Found a valid translation in the TLB.
    } else if (translatePageTable(&address, accessMode, privileges) == 0) {
        printf("Translated            : 0x%08"PRIx32" (%c,%c) --> 0x%08"PRIx32"\n",
                virtual, rw, priv, address);

        return address;       // Found a valid translation in the page table.
    } else {
        printf("Translated            : 0x%08"PRIx32" (%c,%c) --> <invalid>\n",
               virtual, rw, priv);

        return WRONG_ADDRESS; // The address does not have a valid mapping.
    }
}

int main()
{
    test_start("page_table.c");

    // Set up an example page table structure
    basePageDirectory.entries[0] = pointerToInt(&pageTable1) | PAGE_PRESENT_MASK;
    basePageDirectory.entries[(uint32_t) 0x10000000ll
                              >> (BITS_PER_ENTRY + OFFSET_BITS)] = pointerToInt(
                                      &pageTable2) | PAGE_PRESENT_MASK;

    pageTable1.entries[0] = 0x02000000 | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK | PAGE_PRESENT_MASK;
    pageTable1.entries[1] = 0x02001000 | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK | PAGE_PRESENT_MASK;
    pageTable1.entries[2] = 0x02002000 | PAGE_USERMODE_MASK | PAGE_PRESENT_MASK;
    pageTable1.entries[3] = 0x02003000 | PAGE_USERMODE_MASK | PAGE_PRESENT_MASK;
    pageTable2.entries[0] = 0x02010000 | PAGE_USERMODE_MASK | PAGE_READWRITE_MASK | PAGE_PRESENT_MASK;
    // Kernel page
    pageTable2.entries[1] = 0x02020000 | PAGE_PRESENT_MASK;
    pageTable2.entries[1023] = 0x02030000 | PAGE_PRESENT_MASK;

    _dumpPageDirectory(&basePageDirectory);

    setPageDirectory(&basePageDirectory);

    test_equals_int(_doAddressConversion(0x00000000, ACCESS_READ, KERNEL_MODE),
            0x02000000,
            "read access to 0x00000000 in kernel-mode");
    // The access bit should now be set.
    test_equals_int(pageTable1.entries[0] & PAGE_ACCESSED_MASK, PAGE_ACCESSED_MASK, "The access bit is now set");

    test_equals_int(_doAddressConversion(0x00001000, ACCESS_WRITE, USER_MODE), 0x02001000,
            "write access to 0x00001000 in user-mode");
    test_equals_int(_doAddressConversion(0x00002000, ACCESS_WRITE, USER_MODE), -1,
            "write access to 0x00002000 in user-mode");
    test_equals_int(_doAddressConversion(0x00003000, ACCESS_READ, USER_MODE), 0x02003000,
            "read access to 0x00003000 in user-mode");
    test_equals_int(_doAddressConversion(0x10000000, ACCESS_READ, USER_MODE), 0x02010000,
            "read access to 0x10000000 in user-mode");
    test_equals_int(_doAddressConversion(0x10001000, ACCESS_READ, USER_MODE), -1,
            "read access to 0x10001000 in user-mode");
    test_equals_int(_doAddressConversion(0x103ff012, ACCESS_WRITE, KERNEL_MODE), 0x02030012,
            "write access to 0x103ff012 in kernel-mode");

    // Some misses:
    test_equals_int(_doAddressConversion(0x103fe012, ACCESS_WRITE, KERNEL_MODE), -1,
            "write access to 0x103fe012 in kernel-mode");
    test_equals_int(_doAddressConversion(0xfff000ff, ACCESS_READ, KERNEL_MODE), -1,
            "read access to 0xfff000ff in kernel-mode");
    test_equals_int(_doAddressConversion(0x103fe012, ACCESS_WRITE, USER_MODE), -1,
            "write access to 0x103fe012 in user-mode");

    // These values should come from the TLB
    // TODO: Doesn't seem to be the case with the solution.
    test_equals_int(_doAddressConversion(0x103ff012, ACCESS_WRITE, KERNEL_MODE), 0x02030012,
            "write access to 0x103ff012 in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x103ff010, ACCESS_WRITE, KERNEL_MODE), 0x02030010,
            "write access to 0x103ff010 in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x10001000, ACCESS_READ, KERNEL_MODE), 0x02020000,
            "read access to 0x10001000 in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x10001000, ACCESS_WRITE, KERNEL_MODE), 0x02020000,
            "write access to 0x10001000 in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x10000000, ACCESS_WRITE, KERNEL_MODE), 0x02010000,
            "write access to 0x10000000 in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x00003111, ACCESS_READ, USER_MODE), 0x02003111,
            "read access to 0x00003111 in user-mode (TLB)");

    // Evict some pages from the TLB
    test_equals_int(_doAddressConversion(0x00000000, ACCESS_READ, KERNEL_MODE), 0x02000000,
            "read access to 0x00000000 in kernel-mode");
    test_equals_int(_doAddressConversion(0x000010ff, ACCESS_READ, KERNEL_MODE), 0x020010ff,
            "read access to 0x000010ff in kernel-mode");

    // These values should come from the TLB
    test_equals_int(_doAddressConversion(0x10000fff, ACCESS_READ, KERNEL_MODE), 0x02010fff,
            "read access to 0x10000fff in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x00003001, ACCESS_READ, KERNEL_MODE), 0x02003001,
            "read access to 0x00003001 in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x00000001, ACCESS_READ, KERNEL_MODE), 0x02000001,
            "read access to 0x00000001 in kernel-mode (TLB)");
    test_equals_int(_doAddressConversion(0x00001000, ACCESS_READ, KERNEL_MODE), 0x02001000,
            "read access to 0x00001000 in kernel-mode (TLB)");

    // Replace an entry in existing page table.
    mapPage(0x00001000, 0x02008000, ACCESS_WRITE, USER_MODE);
    // This should not be read from the TLB.
    test_equals_int(_doAddressConversion(0x00001001, ACCESS_READ, KERNEL_MODE), 0x02008001,
            "read access to 0x00001001 in kernel-mode (not from TLB)");
    // This should still be in the TLB
    test_equals_int(_doAddressConversion(0x00000001, ACCESS_READ, KERNEL_MODE), 0x02000001,
            "read access to 0x00000001 in kernel-mode (TLB)");

    // Add a new entry to existing page table
    mapPage(0x00004000, 0x03004000, ACCESS_READ, USER_MODE);

    // Add a new page table
    mapPage(0x01004000, 0x02004000, ACCESS_WRITE, USER_MODE);

    test_equals_int(_doAddressConversion(0x00004eee, ACCESS_READ, USER_MODE), 0x03004eee,
            "read access to 0x00004eee in user-mode");
    test_equals_int(_doAddressConversion(0x00004eee, ACCESS_WRITE, USER_MODE), -1,
            "write access to 0x00004eee in user-mode");
    test_equals_int(_doAddressConversion(0x01004abc, ACCESS_WRITE, USER_MODE), 0x02004abc,
            "write access to 0x01004abc in user-mode");

    _dumpPageDirectory(&basePageDirectory);

    return test_end();
}
