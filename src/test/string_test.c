#ifdef TEST_MODE
#include "string.h"
#include "test/assert.h"
#include "test/test.h"

void test_str_equals() {
    const char* a = "Hello";
    const char* b = "Hello";
    const char* c = "Hello ";
    const char* d = "hi!";

    describe(
        "str_equals",
        assert(str_equals(a, b), "a, b"),
        assert(!str_equals(a, c), "a, c"),
        assert(!str_equals(a, d), "a, d"),
        assert(!str_equals(c, d), "c, d")
    );
}

#define SIZE 8
void test_itoa() {
    char buf1[SIZE];
    char buf2[SIZE];
    char buf3[SIZE];
    char buf4[SIZE];
    char buf5[SIZE];

    itoa(12345, buf1);
    itoa(-120, buf2);
    itoa(-14, buf3);
    itoa(12, buf4);
    itoa(1234, buf5);

    describe(
        "itoa",
        assert_equals_str("12345", buf1, ""),
        assert_equals_str("-120", buf2, ""),
        assert_equals_str("-14", buf3, ""),
        assert_equals_str("12", buf4, ""),
        assert_equals_str("1234", buf5, "")
    );
}

void test_itoa_hex() {
    char buf1[SIZE];
    char buf2[SIZE];
    char buf3[SIZE];
    char buf4[SIZE];
    
    itoa_hex(0xFFFF, buf1);
    itoa_hex(0xABC123, buf2);
    itoa_hex(0x19B, buf3);
    itoa_hex(0x0, buf4);

    describe(
        "convert uint to hex string",
        assert_equals_str("FFFF", buf1, "buf1"),
        assert_equals_str("ABC123", buf2, "buf2"),
        assert_equals_str("19B", buf3, "buf3"),
        assert_equals_str("0", buf4, "buf4")
    );

}

void test_string_h() {
    test_itoa();
    test_itoa_hex();
    test_str_equals();
}

#endif
