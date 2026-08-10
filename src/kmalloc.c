#include "hardware/allocator.h"
#include "stdint.h"

struct PageMeta {
    usize block_size;
    u64 total_allocated;
    struct PageMeta *next_page;
    uptr next_free;
};

#define BLOCK_COUNT(meta)                                                      \
    (PAGE_SIZE - sizeof(struct PageMeta)) / (meta).block_size
#define PAGE_FULL(meta) (BLOCK_COUNT(meta) == (meta).total_allocated)

// vm spec requires pages to be aligned by PAGE_SIZE
#define INFER_PAGE(ptr) ((ptr) - ((ptr) % PAGE_SIZE))

static struct PageMeta *free_8 = NULL;
static struct PageMeta *free_16 = NULL;
static struct PageMeta *free_32 = NULL;
static struct PageMeta *free_48 = NULL;
static struct PageMeta *free_64 = NULL;
static struct PageMeta *free_128 = NULL;
static struct PageMeta *free_256 = NULL;
static struct PageMeta *free_512 = NULL;
static struct PageMeta *free_1024 = NULL;

void *kmalloc(usize size) {}

void kfree(void *ptr) {}

#ifdef TEST_MODE
#include "debug.h"
#include "test/assert.h"

void test_malloc()
{
    struct PageMeta test_meta = {.next_page = NULL,
                                 .next_free = 0,
                                 .block_size = 8,
                                 .total_allocated = 0};

    test_meta.total_allocated = BLOCK_COUNT(test_meta);

    pageframe_t test_frame = kalloc_frame();
    debug_printf("test frame %x\n", test_frame);
    debug_printf("test frame_ %x\n",
                 (test_frame + 48) - ((test_frame + 48) % PAGE_SIZE));

    describe("test kmalloc page macros",
             assert_equals_uint(508, BLOCK_COUNT(test_meta),
                                "correctly calculates block count"),
             assert(PAGE_FULL(test_meta), "checks if a page is full"),
             assert_equals_ptr((void *)test_frame,
                               (void *)INFER_PAGE(test_frame + 48),
                               "aligns a random pointer to a page"));

    kfree_frame(test_frame);
}

#endif
