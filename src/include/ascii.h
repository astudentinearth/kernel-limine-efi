#pragma once
static inline char digit_to_ascii(int i) {
    if(i < 0) return '?';
    if(i < 10) return i + '0';   
    return 'A' + (i % 10); // handle hex
}

