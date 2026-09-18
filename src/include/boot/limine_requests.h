#pragma once
#include "hardware/display.h"
#include "limine.h"
#include "stdint.h"
#include <stdbool.h>

/** @deprecated */
struct limine_framebuffer *get_limine_framebuffer(int i);

display_t limine_get_display(usize n);

struct limine_memmap_response *get_limine_memmap();
u64 get_framebuffer_count();
bool is_base_revision_supported();
bool limine_framebuffer_available();
u64 get_hhdm_offset();
u64 get_physical_executable_base();
u64 get_virtual_executable_base();
struct limine_rsdp_response *get_limine_rsdp();
uptr limine_get_rsdp_base();
