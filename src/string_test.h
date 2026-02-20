
#pragma once
#include "debug.h"
#ifdef TEST_MODE

#include "string.h"
#include "test/assert.h"


static inline void test_strcmp() {
    debug("[TEST] Testing strcmp");
    const char* a = "Hello";
    const char* b = "Hello";
    const char* c = "Hello ";
    const char* d = "hi!";

    if(
            strcmp(a, b) &&
            !strcmp(a, c) &&
            !strcmp(a, d) &&
            !strcmp(c, d)
      )    {
        debug("[TEST] strcmp test PASS.");
    }
    else {
        debug("[TEST] strcmp test FAIL.");
    }
}

static inline void test_itoa() {
#define SIZE 32
    debug("[TEST] Testing itoa");
    char buf[SIZE];

    itoa(12345, buf);
    if(!strcmp(buf, "12345")) goto fail;
    memset(buf, 0, SIZE);

    itoa(-120, buf);
    if(!strcmp(buf, "-120")) goto fail;
    memset(buf, 0, SIZE);

    itoa(-14, buf);
    if(!strcmp(buf, "-14")) goto fail;
    memset(buf, 0, SIZE);


    itoa(12, buf);
    if(!strcmp(buf, "12")) goto fail;
    memset(buf, 0, SIZE);


    itoa(1234, buf);
    if(!strcmp(buf, "1234")) goto fail;
    memset(buf, 0, SIZE);

    debug("[TEST] itoa test PASS.");
    return;
    
fail:
    debug("[TEST] itoa test FAIL.");
}

static inline void test_itoa_hex() {
    debug("[TEST] Testing itoa_hex");
    char buf1[SIZE];
    char buf2[SIZE];
    char buf3[SIZE];
    char buf4[SIZE];

    itoa_hex(0xFFFF, buf1);
    itoa_hex(0xABC123, buf2);
    itoa_hex(0x19B, buf3);
    itoa_hex(0x0, buf4);

    bool pass =
        assert_equals_str("FFFF", buf1, "itoa_hex")
        && assert_equals_str("ABC123", buf2, "itoa_hex")
        && assert_equals_str("19B", buf3, "itoa_hex")
        && assert_equals_str("0", buf4, "itoa_hex");

    if(pass) debug("[TEST] itoa_hex test PASS.");
    else debug("[TEST] itoa_hex test FAIL.");

}

static inline void test_string_h() {
    test_strcmp();
    test_itoa();
    test_itoa_hex();
}

#endif

