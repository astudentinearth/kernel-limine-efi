
#ifdef TEST_MODE
#include "debug.h"
#endif


__attribute__((noreturn))
void exception_handler(void);
void exception_handler() {
#ifdef TEST_MODE
    debug("panic");
#endif
    __asm__ volatile ("cli; hlt");
}

