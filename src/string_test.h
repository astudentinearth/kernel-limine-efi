
#pragma once
#include "debug.h"
#ifdef TEST_MODE

#include "string.h"


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

static inline void test_string_h() {
    test_strcmp();
    test_itoa();
}

#endif

