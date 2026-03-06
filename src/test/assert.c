#ifdef TEST_MODE
#include "test/assert.h"
#include "debug.h"
#include <stdarg.h>
#include "string.h"
#include "term.h"



bool _assert(const char* file, int line, bool actual, const char *label) {
    if(!actual) {
        debug_err("[FAIL] [%s:%d] [%s] Assertion failed (received false)\n", file, line, label);
    }
    return actual;
}

bool _assert_equals_uint(const char* file, int line, uint64_t expected, uint64_t actual, const char *label) {
    if(expected != actual) {
        debug_printf("[FAIL] [%s:%d] [%s] Expected: %u | Received: %u\n", file, line, label, expected, actual);
        return false;
    }
    return true;
}

bool _assert_equals_str(const char* file, int line, const char *expected, const char *actual, const char *label) {
    bool success = str_equals(expected, actual);
    if(!success) {
        debug_printf("[FAIL] [%s:%d] [%s] Expected: \"%s\" | Received: \"%s\"\n", file, line, label, expected, actual);
    }
    return success;
}

void _test_pass(const char *file, int line, const char* msg) {
    debug_success("[PASS] [%s:%d] [%s]\n", file, line, msg);
}

bool _describe(const char *file, int line, const char *name, ...) {
    int pass = 0;
    int fail = 0;

    va_list args;
    va_start(args, name);

    int current = va_arg(args, int);
    while(current != END_DESCRIBE) {
        if(current == 1) pass++;
        else fail++;
        current = va_arg(args, int);
    }

    if(fail > 0) {
        debug_err("[FAIL:%s] [%s:%d] | %d passed | %d failed | Total: %d\n", name, file, line, pass, fail, pass+fail);
    }
    else {
        debug_success("[PASS:%s] [%s:%d] | %d passed\n", name, file, line, pass);
    }

    va_end(args);
    return fail == 0;
}

#endif
