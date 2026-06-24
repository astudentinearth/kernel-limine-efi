#pragma once

#include <stdint.h>
typedef struct {
    uintptr_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
} Framebuffer_t;

