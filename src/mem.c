
#include "string.h"

typedef u64 copy_word_t __attribute__((__may_alias__, __aligned__(1)));

void *memcpy(void *dest, const void *src, usize n)
{
    u8 *src_u8 = (u8*)src;
    u8 *dest_u8 = (u8*)dest;

    // copy until we are word aligned
    while((uptr)src_u8 % sizeof(copy_word_t) && n > 0) {
        *dest_u8 = *src_u8;
        src_u8++;
        dest_u8++;
        n--;
    }

    if(n == 0) return dest;

    copy_word_t *src_u64 = (copy_word_t*)src_u8;
    copy_word_t *dest_u64 = (copy_word_t*)dest_u8;

    // copy words
    while(n >= sizeof(copy_word_t)) {
        *dest_u64 = *src_u64;
        src_u64++;
        dest_u64++;
        n -= sizeof(copy_word_t);
    }

    if(n == 0) return dest;

    src_u8 = (u8*)src_u64;
    dest_u8 = (u8*)dest_u64;

    for(usize i = 0; i < n; i++) dest_u8[i] = src_u8[i];

    return dest;
}

void *memset(void *s, int c, usize n)
{
    u8 *p = (u8 *)s;

    for (usize i = 0; i < n; i++) {
        p[i] = (u8)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, usize n)
{
    u8 *pdest = (u8 *)dest;
    const u8 *psrc = (const u8 *)src;

    if (src > dest) {
        for (usize i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (usize i = n; i > 0; i--) {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, usize n)
{
    const u8 *p1 = (const u8 *)s1;
    const u8 *p2 = (const u8 *)s2;

    for (usize i = 0; i < n; i++) {
        if (p1[i] != p2[i]) { return p1[i] < p2[i] ? -1 : 1; }
    }

    return 0;
}
