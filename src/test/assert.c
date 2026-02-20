#ifdef TEST_MODE
#include "assert.h"
#include "../debug.h"



bool assert(bool actual, const char *label) {
    if(!actual) {
        debug_puts("[FAIL] Assertion failed (");
        debug_puts(label);
        debug_puts(")\n");
    }
    return actual;
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
