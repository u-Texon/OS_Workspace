#include "testlib.h"
#include "strings.h"



int main()
{
    test_start("strings.c");

    char *hello = "Hello";
    test_equals_int(stringlength(hello), 5, "stringlength works for 'Hello'");
    char *world = " World!";
    test_equals_int(stringlength(world), 7, "stringlength works for ' World!'");
    char *helloWorld = stringconcat(hello, " World!");
    test_equals_string(helloWorld, "Hello World!", "stringconcat works for 'Hello' and ' World!'");

    char **helloAndWorld = stringsplit(helloWorld, 'o');
    test_equals_string(helloAndWorld[0], "Hell", "stringsplit for 'Hell' works");
    test_equals_string(helloAndWorld[1], " W", "stringsplit for ' W' works");
    test_equals_string(helloAndWorld[2], "rld!", "stringsplit for 'rld!' works");
    test_equals_ptr(helloAndWorld[3], NULL, "stringsplit for 'Hello World!' with delimiter ' ' returns result that is correctly terminated by NULL pointer");

    free(helloWorld);
    stringsplit_free(helloAndWorld);

    return test_end();
}
