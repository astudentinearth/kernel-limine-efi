
#include "limine.h"
#include <stdbool.h>
#include <stdint.h>

struct limine_framebuffer* get_limine_framebuffer(int i);
struct limine_memmap_response* get_limine_memmap();
uint64_t get_framebuffer_count();
bool is_base_revision_supported();
bool limine_framebuffer_available();

