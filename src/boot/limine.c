
#include "boot/limine.h"
#include "boot/limine_requests.h"
#include "debug.h"
#include <stddef.h>

__attribute__((used,
               section(".limine_requests_"
                       "start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_memmap_request
    memmap_request = {.id = LIMINE_MEMMAP_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_rsdp_request
    rsdp_request = {.id = LIMINE_RSDP_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_hhdm_request
    hhdm_request = {.id = LIMINE_HHDM_REQUEST, .revision = 0};

__attribute__((used, section(".limine_requests"))) static volatile struct
    limine_executable_address_request executable_address_request = {
        .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST, .revision = 0};

__attribute__((
    used,
    section(
        ".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

bool limine_framebuffer_available()
{ return framebuffer_request.response != NULL && get_framebuffer_count() > 0; }

struct limine_framebuffer *get_limine_framebuffer(int i)
{ return framebuffer_request.response->framebuffers[i]; }

display_t limine_get_display(usize n) {
    struct limine_framebuffer *fb = get_limine_framebuffer(n);
    display_t display = {
        .fb_address = fb->address,
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch,
        .bpp = fb->bpp
    };

    return display;
}

u64 get_framebuffer_count()
{ return framebuffer_request.response->framebuffer_count; }

u64 get_hhdm_offset()
{
    struct limine_hhdm_response *response = hhdm_request.response;
    return response->offset;
}

u64 get_physical_executable_base()
{
    struct limine_executable_address_response *response =
        executable_address_request.response;
    return response->physical_base;
}

u64 get_virtual_executable_base()
{
    struct limine_executable_address_response *response =
        executable_address_request.response;
    return response->virtual_base;
}

struct limine_memmap_response *get_limine_memmap()
{ return memmap_request.response; }

struct limine_rsdp_response *get_limine_rsdp() { return rsdp_request.response; }

bool is_base_revision_supported() { return LIMINE_BASE_REVISION_SUPPORTED; }

uptr limine_get_rsdp_base()
{
    if (rsdp_request.response == NULL) {
        panic("Limine RSDP response is null.");
    }
    return rsdp_request.response->address;
}
