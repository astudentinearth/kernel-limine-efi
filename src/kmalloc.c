#include "debug.h"
#include "hardware/allocator.h"
#include "paging.h"
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
#define BLOCK_OFFSET(ptr, bs, n)                                               \
    ((ptr) + (sizeof(struct PageMeta)) + ((bs) * (n)))

static struct PageMeta *free_8 = NULL;
static struct PageMeta *free_16 = NULL;
static struct PageMeta *free_32 = NULL;
static struct PageMeta *free_48 = NULL;
static struct PageMeta *free_64 = NULL;
static struct PageMeta *free_128 = NULL;
static struct PageMeta *free_256 = NULL;
static struct PageMeta *free_512 = NULL;
static struct PageMeta *free_1024 = NULL;

struct PageMeta *initialize_page(pageframe_t _page_ptr, usize bs)
{
    uptr page_ptr = (uptr)get_virtaddr((void *)_page_ptr);
    debug_info("initializing page at %x with bs=%u\n", page_ptr, bs);
    struct PageMeta *meta = (struct PageMeta *)page_ptr;
    meta->total_allocated = 0;
    meta->block_size = bs;
    meta->next_page = NULL;
    debug_info(">> setting up free list\n");
    // set up the free list
    for (u64 i = 0; i < BLOCK_COUNT(*meta); i++) {
        uptr addr = BLOCK_OFFSET(page_ptr, bs, i);
        uptr *ptr = (uptr *)addr;
        *ptr = addr + bs;
    }

    debug_info(">> terminating free list\n");
    // set last slot to null (free list end)
    uptr last_ptr = BLOCK_OFFSET(page_ptr, bs, BLOCK_COUNT(*meta) - 1);
    *((uptr *)last_ptr) = (uptr)NULL;

    meta->next_free = (uptr)meta + sizeof(struct PageMeta);
    debug_info(">> %x free list head\n", meta->next_free);

    return meta;
}

void *kmalloc(usize size) {}

void kfree(void *ptr) {}

#ifdef TEST_MODE
#include "debug.h"
#include "test/assert.h"

void test_malloc()
{
    pageframe_t test_frame = kalloc_frame();

    {
        struct PageMeta test_meta = {.next_page = NULL,
                                     .next_free = 0,
                                     .block_size = 8,
                                     .total_allocated = 0};

        test_meta.total_allocated = BLOCK_COUNT(test_meta);

        describe("test kmalloc page macros",
                 assert_equals_uint(508, BLOCK_COUNT(test_meta),
                                    "correctly calculates block count"),
                 assert(PAGE_FULL(test_meta), "checks if a page is full"),
                 assert_equals_ptr((void *)test_frame,
                                   (void *)INFER_PAGE(test_frame + 48),
                                   "aligns a random pointer to a page"),
                 assert_equals_uint(BLOCK_OFFSET(0, 8, 2),
                                    sizeof(struct PageMeta) + 16,
                                    "calculates the offset for a slot")

        );
    }

    // test frame init

    struct PageMeta *test_meta = initialize_page(test_frame, 8);
    debug_printf("begin page init test");
    describe("test page initialization",
             assert_equals_ptr(test_meta, get_virtaddr((void *)test_frame),
                               "returns the correct frame pointer"),
             assert_equals_ptr(NULL, (void *)test_meta->next_page,
                               "next page initially set to null"),
             assert_equals_ptr((void *)BLOCK_OFFSET((uptr)test_meta, 8, 0),
                               (void *)test_meta->next_free,
                               "first slot is immediately after header"),
             assert_equals_uint(
                 8, ((*(uptr *)test_meta->next_free) - test_meta->next_free),
                 "second slot - first block = block size on init"),
             assert_equals_uint(
                 0,
                 *(uptr *)((BLOCK_OFFSET((uptr)test_meta, test_meta->block_size,
                                         BLOCK_COUNT(*test_meta) - 1))),
                 "sets the last slot to NULL")

    );

    kfree_frame(test_frame);
}

#endif
