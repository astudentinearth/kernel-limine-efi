
#include "limine.h"
#include "boot/limine_requests.h"
#include <stddef.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

bool limine_framebuffer_available() {
    return framebuffer_request.response != NULL && get_framebuffer_count() > 0;
}

struct limine_framebuffer* get_limine_framebuffer(int i) {
    return framebuffer_request.response->framebuffers[i];
}

uint64_t get_framebuffer_count() {
    return framebuffer_request.response->framebuffer_count;
}

struct limine_memmap_response* get_limine_memmap() {
    return memmap_request.response;
}

bool is_base_revision_supported() {
    return LIMINE_BASE_REVISION_SUPPORTED;
}

