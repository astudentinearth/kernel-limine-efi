#pragma once
#include "limine.h"
#include <stdbool.h>
#include "stdint.h"

struct limine_framebuffer* get_limine_framebuffer(int i);
struct limine_memmap_response* get_limine_memmap();
u64 get_framebuffer_count();
bool is_base_revision_supported();
bool limine_framebuffer_available();
u64 get_hhdm_offset();
u64 get_physical_executable_base();
u64 get_virtual_executable_base();
struct limine_rsdp_response* get_limine_rsdp();
uptr limine_get_rsdp_base();

