
#include <stdint.h>

// page tree offsets
#define PML4_IDX(virt)  (virt >> 39) & 0x1FF
#define PDPT_IDX(virt)  (virt >> 30) & 0x1FF
#define PD_IDX(virt)    (virt >> 21) & 0x1FF
#define PT_IDX(virt)    (virt >> 12) & 0x1FF
#define PAGE_OFFSET(virt) (virt & 0xFFF)
#define ENTRY_ADDR(entry)   (entry & 0x000FFFFFFFFFF000ULL)


// flags
#define IS_PRESENT 0x1
#define READ_WRITE 0x2


