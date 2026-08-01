
#ifdef TEST_MODE
#include "debug.h"
#include "string.h"
#include "test/assert.h"

struct TestBlock {
    int x;
    int y;
    char z;
};

void test_memcpy() {
    struct TestBlock b1;
    struct TestBlock b2;

    b1.x = 12;
    b1.y = 24;
    b1.z = 'c';

    b2.x = 4;
    b2.y = 48;
    b2.z = 'q';

    memcpy(&b2, &b1, sizeof(struct TestBlock));

    describe(
        "memcpy", 
        assert(b2.x == 12, "x"), 
        assert(b2.y == 24, "y"), 
        assert(b2.z == 'c', "c")
    );


}
#endif
