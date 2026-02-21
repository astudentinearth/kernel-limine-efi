
#ifdef TEST_MODE
#include "debug.h"
#include "mem.h"

struct TestBlock {
    int x;
    int y;
    char z;
};

static inline void test_memcpy() {
    debug("[TEST] Testing memcpy");
    struct TestBlock b1;
    struct TestBlock b2;

    b1.x = 12;
    b1.y = 24;
    b1.z = 'c';

    b2.x = 4;
    b2.y = 48;
    b2.z = 'q';

    memcpy(&b2, &b1, sizeof(struct TestBlock));
    
    if(b2.x != 12 || b2.y != 24 || b2.z != 'c') {
        debug("[TEST] memcpy test FAIL.");
    }
    else {
        debug("[TEST] memcpy test PASS.");
    }

}
#endif
