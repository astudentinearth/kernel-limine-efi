#pragma once
#include "stdint.h"

#define INTERRUPTS_ENABLED 0x200
#define INTERRUPTS_DISABLED 0x0
extern u64 get_interrupt_flag();

static inline void __restore_if(u64 *_if) {
    if(*_if == INTERRUPTS_ENABLED) __asm__ volatile("sti" ::: "memory");
}

#define _no_interrupts \
    u64 __attribute__((cleanup(__restore_if))) ___if_org = get_interrupt_flag(); \
    __asm__ volatile("cli" ::: "memory"); 

