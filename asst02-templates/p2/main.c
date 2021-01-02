#include "testlib.h"
#include "hash.h"


int main()
{
    test_start("hash.c");

    test_equals_int64(hashString("A"), 177638, "hash of A is correct");

    element e[10] = { {0} };
    hashtable h = {e, 10};

    test_equals_int(insert(&h, "abc", 1234), 1, "inserting abc works");
    int val;
    test_equals_int(find(&h, "abc", &val), 1, "retrieving abc works");
    test_equals_int(val, 1234, "h[abc] == 1234");

    //my tests
    test_equals_int(find(&h, "lol", &val), 0, "elemnt can't be found");
    test_equals_int(insert(&h, "abc", 1001), 1, "overwriting abc works");

    test_equals_int(insert(&h, "d", 1), 1, "inserting d works");

    test_equals_int(insert(&h, "d", 2), 1, "overwriting d works");

    test_equals_int(insert(&h, "e", 1), 1, "inserting e works");
    test_equals_int(insert(&h, "f", 1), 1, "inserting f works");
    test_equals_int(insert(&h, "g", 1), 1, "inserting g works");
    test_equals_int(insert(&h, "h", 1), 1, "inserting h works");
    test_equals_int(insert(&h, "i", 1), 1, "inserting i works");
    test_equals_int(insert(&h, "j", 1), 1, "inserting j works");
    test_equals_int(insert(&h, "k", 1), 1, "inserting k works");
    test_equals_int(insert(&h, "m", 1), 0, "array is full works");
    


    return test_end();
}
