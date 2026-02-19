#pragma once
#include <stdint.h>
#include <stddef.h>
#include "debug.h"

inline void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

inline void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

inline void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

inline int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

#ifdef TEST_MODE
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

