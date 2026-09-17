
#pragma once
#include "stdint.h"

#define VMM_CAP_PHYSICAL_ADDRESS_BITS 46

/** Initialize the higher-half virtual address arena at hhdm_offset +
 * 2^physical_address_bits. Max bits are capped at
 * VMM_CAP_PHYSICAL_ADDRESS_BITS. */
void init_vmm(usize hhdm_offset, usize physical_address_bits);

/** Reserve n bytes in the higher-half virtual address space. **This does not map pages, you have to handle it yourself!** */
void *valloc(usize n);
usize get_vmm_start();
