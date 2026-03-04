#include <stdbool.h>
#define PIC1_8259_COMMAND   0x0020
#define PIC1_8259_DATA      0x0021
#define PIC2_8259_COMMAND   0x00A0
#define PIC2_8259_DATA      0x00A1

void disable_legacy_pic(void);
bool check_apic(void);

