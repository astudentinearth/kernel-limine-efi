
static inline char digit_to_ascii(int i) {
    if(i > 10 || i < 0) return '?';
    return i + '0';   
}

