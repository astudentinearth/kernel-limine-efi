#pragma once
#include "serial.h"

inline void debug(const char* msg) {
    for(const char* c = msg; *c != 0; c++) {
        outb(COM1, *c);
    }
    outb(COM1, '\n');
}
