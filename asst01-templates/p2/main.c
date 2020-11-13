#include "testlib.h"
#include "parseint.h"

int main()
{
    test_start("parseint.c");

    test_equals_int(parseDecimalChar('0'), 0, "parseDecimalChar parses '0'");
    test_equals_int(parseDecimalChar('4'), 4, "parseDecimalChar parses '4'");
    test_equals_int(parseDecimalChar('f'), -1, "parseDecimalChar parses 'f'");

    test_equals_int(parseInt("010"), 8, "parseInt parses octal number");
    test_equals_int(parseInt("0165"), 117, "parseInt parses octal number: 0165");
    test_equals_int(parseInt("10"), 10, "parseInt parses decimal number");
    test_equals_int(parseInt("12345"), 12345, "parseInt parses decimal number: 12345");
    test_equals_int(parseInt("?"), -1, "parseInt handles invalid input");
    test_equals_int(parseInt(""), -1, "parseInt handles invalid input: empty string");

    return test_end();
}
