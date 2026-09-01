#pragma once
#include "stdint.h"
#include <stdbool.h>

#define option_empty true
#define option_not_empty false

#define DEFINE_OPTION(T)                                                       \
    typedef struct option_##T {                                                \
        T val;                                                                 \
        bool empty;                                                            \
    } option_##T;

// frequently used options
DEFINE_OPTION(u64);
DEFINE_OPTION(uptr);
DEFINE_OPTION(usize);
