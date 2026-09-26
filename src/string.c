#include "string.h"
#include "ascii.h"
#include "math.h"
#include "result.h"

void itoa(int64_t num, char *buf)
{
    if (num == 0) {
        buf[0] = '0';
        buf[1] = 0;
        return;
    }

    int64_t value = abs(num);
    int digit_count = 0;
    char *cur = buf;

    if (num < 0) { *cur++ = '-'; }

    // convert to digits in reverse
    while (value > 0) {
        int64_t remainder = value % 10;
        value /= 10;
        *cur++ = digit_to_ascii(remainder);
        digit_count++;
    }

    *cur = 0;

    char *seq = num < 0 ? buf + 1 : buf;

    for (int i = 0; i < digit_count / 2; i++) {
        char b = seq[digit_count - 1 - i];
        seq[digit_count - 1 - i] = seq[i];
        seq[i] = b;
    }
}

void itoa_u(u64 num, char *buf)
{
    if (num == 0) {
        buf[0] = '0';
        buf[1] = 0;
        return;
    }

    int digit_count = 0;
    char *cur = buf;

    // convert to digits in reverse
    while (num > 0) {
        u64 remainder = num % 10;
        num /= 10;
        *cur++ = digit_to_ascii(remainder);
        digit_count++;
    }

    *cur = 0;
    char *seq = buf;

    for (int i = 0; i < digit_count / 2; i++) {
        char b = seq[digit_count - 1 - i];
        seq[digit_count - 1 - i] = seq[i];
        seq[i] = b;
    }
}

void itoa_hex(u64 num, char *buf)
{

    if (num == 0) {
        buf[0] = '0';
        buf[1] = 0;
        return;
    }

    int digit_count = 0;
    char *cur = buf;

    // convert to digits in reverse
    while (num > 0) {
        u64 remainder = num % 16;
        num /= 16;
        *cur++ = digit_to_ascii(remainder);
        digit_count++;
    }

    *cur = 0;
    char *seq = buf;

    for (int i = 0; i < digit_count / 2; i++) {
        char b = seq[digit_count - 1 - i];
        seq[digit_count - 1 - i] = seq[i];
        seq[i] = b;
    }
}

bool str_equals(const char *a, const char *b)
{
    while (*a != 0 || *b != 0) {
        if (*a++ != *b++) { return false; }
    }
    return true;
}

#define __PARSE_UINT_MAX_DIGITS 20

option_u64 parse_uint(const char *str)
{
    option_u64 _none = None(u64);

    usize len = 0;
    while (str[len] != 0) {
        if (++len > __PARSE_UINT_MAX_DIGITS) {
            return _none;
        }
    }

    if (len == 0) { return _none; }

    u64 result = 0;
    for(usize i = 0; i < len; i++) {
        u8 ch = str[i];
        if(!is_digit(ch)) return _none;
        u64 digit = ch - '0';
        if(result > (U64_MAX - digit) / 10) return _none;
        result = result * 10 + digit;
    }

    return Some(u64, result);
}
