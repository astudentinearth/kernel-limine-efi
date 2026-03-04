#ifdef TEST_MODE
#include "test/assert.h"
#include "debug.h"
#include "string.h"



bool assert(bool actual, const char *label) {
    if(!actual) {
        debug_puts("[FAIL] Assertion failed (");
        debug_puts(label);
        debug_puts(")\n");
    }
    return actual;
}

bool assert_equals_uint(uint64_t expected, uint64_t actual, const char *label) {
    if(expected != actual) {
        debug_printf("[FAIL] Assertion failed | Expected: %u | Actual: %u (%s)\n", expected, actual, label);
        return false;
    }
    return true;
}

bool assert_equals_str(const char *expected, const char *actual, const char *label) {
    bool success = str_equals(expected, actual);
    if(!success) {
        debug_puts("[FAIL] Assertion failed (");
        debug_puts(label);
        debug_puts(") - Expected: ");
        debug_puts(expected);
        debug_puts(" | Received: ");
        debug_puts(actual);
        debug_puts("\n");
    }
    return success;
}

#endif
