
#include "hardware/io.h"
#include "debug.h"

#define QUEUE_SIZE 512
static kinterrupt_t queue[QUEUE_SIZE];
static volatile u64 head = 0;
static volatile u64 tail = 0;
kinterrupt_t noop = {
    .payload = NOOP,
    .type = NOOP
};

void queue_interrupt(kinterrupt_t interrupt) {
    __asm__ volatile("cli");
    u64 next = (head+1) % QUEUE_SIZE;
    if (next == tail) {
        __asm__ volatile("sti");
        return;
    }
    queue[head] = interrupt;
    head = next;
    __asm__ volatile("sti");
}

void kern_handle_interrupt() {
    while(tail != head) {
        __asm__ volatile("cli");
        kinterrupt_t i = queue[tail];
        tail = (tail + 1) % QUEUE_SIZE;
        __asm__ volatile("sti");
        switch(i.type) {
            case KEYBOARD_INT: {
                process_keyboard_event(i.payload);
                break;
                               }
        }
    }
}


