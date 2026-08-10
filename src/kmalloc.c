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

#define BUCKET_COUNT 9
#define BLOCK_TOO_BIG 9999
usize block_sizes[BUCKET_COUNT] = {8, 16, 32, 48, 64, 128, 256, 512, 1024};

static usize round_up(usize bs)
{
    for (int i = 0; i < BUCKET_COUNT; i++) {
        usize _bs = block_sizes[i];
        if (bs > _bs) { continue; }
        return _bs;
    }
    return BLOCK_TOO_BIG;
}

/** Initializes a page with a free list. */
struct PageMeta *initialize_page(void *_page_ptr, usize bs)
{
    uptr page_ptr = (uptr)_page_ptr;
    // debug_info("initializing page at %x with bs=%u\n", page_ptr, bs);
    struct PageMeta *meta = (struct PageMeta *)page_ptr;
    meta->total_allocated = 0;
    meta->block_size = bs;
    meta->next_page = NULL;
    // debug_info(">> setting up free list\n");
    //  set up the free list
    for (u64 i = 0; i < BLOCK_COUNT(*meta); i++) {
        uptr addr = BLOCK_OFFSET(page_ptr, bs, i);
        uptr *ptr = (uptr *)addr;
        *ptr = addr + bs;
    }

    // debug_info(">> terminating free list\n");
    //  set last slot to null (free list end)
    uptr last_ptr = BLOCK_OFFSET(page_ptr, bs, BLOCK_COUNT(*meta) - 1);
    *((uptr *)last_ptr) = (uptr)NULL;

    meta->next_free = (uptr)meta + sizeof(struct PageMeta);
    // debug_info(">> %x free list head\n", meta->next_free);

    return meta;
}

struct PageMeta **get_page_for_bucket(usize bs)
{
    bs = round_up(bs);
    if (bs == BLOCK_TOO_BIG) {
        return NULL; // reroute to a different allocator
    }
    struct PageMeta **result;
    switch (bs) {
    case 8:
        result = &free_8;
        break;

    case 16:
        result = &free_16;
        break;

    case 32:
        result = &free_32;
        break;

    case 48:
        result = &free_48;
        break;

    case 64:
        result = &free_64;
        break;

    case 128:
        result = &free_128;
        break;

    case 256:
        result = &free_256;
        break;

    case 512:
        result = &free_512;
        break;

    case 1024:
        result = &free_1024;
        break;
    }

    if (*result != NULL) { return result; }

    void *new_frame = kalloc_vframe();
    struct PageMeta *meta = initialize_page(new_frame, bs);
    *result = meta;
    return result;
}

void *fixed_alloc(usize bs)
{
    struct PageMeta **bucket = get_page_for_bucket(bs);
    struct PageMeta *page = *bucket;
    uptr current_free = page->next_free;
    uptr next_free = *(uptr *)current_free;

    page->total_allocated++;
    page->next_free = next_free;

    if (next_free == NULL) {
        *bucket = NULL; 
    }

    return (void *)current_free;
}

void *kmalloc(usize size)
{
    usize bs = round_up(size);
    if (bs == BLOCK_TOO_BIG) {
        return NULL; // route to multi page allocator
    }
    return fixed_alloc(bs);
}

void kfree(void *ptr) {}

#ifdef TEST_MODE
#include "debug.h"
#include "test/assert.h"

void print_free_1024_addr()
{ debug_err("free_1024 addr: %x value: %x\n", &free_1024, free_1024); }

void test_malloc()
{
    print_free_1024_addr();
    {
        pageframe_t test_frame = kalloc_frame();
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
        kfree_frame(test_frame);
    }

    // test frame init

    for (int i = 0; i < BUCKET_COUNT; i++) {
        usize bs = block_sizes[i];
        void *test_frame = kalloc_vframe();
        debug_info(">>> Testing page init with %u block size\n", bs);

        struct PageMeta *test_meta = initialize_page(test_frame, bs);
        usize slot_count = 0;
        {
            uptr *_ptr = (uptr *)test_meta->next_free;
            while (*_ptr != NULL) {
                slot_count++;
                _ptr = (uptr *)*_ptr;
            }
        }

        describe(
            "test page initialization",
            assert_equals_ptr(test_meta, test_frame,
                              "returns the correct frame pointer"),
            assert_equals_ptr(NULL, (void *)test_meta->next_page,
                              "next page initially set to null"),
            assert_equals_ptr((void *)BLOCK_OFFSET((uptr)test_meta, bs, 0),
                              (void *)test_meta->next_free,
                              "first slot is immediately after header"),
            assert_equals_uint(
                bs, ((*(uptr *)test_meta->next_free) - test_meta->next_free),
                "second slot - first block = block size on init"),
            assert_equals_uint(
                0,
                *(uptr *)((BLOCK_OFFSET((uptr)test_meta, test_meta->block_size,
                                        BLOCK_COUNT(*test_meta) - 1))),
                "sets the last slot to NULL"),
            assert_equals_uint(BLOCK_COUNT(*test_meta), slot_count + 1,
                               "the free list is correctly formed")

        );
        kfree_vframe(test_frame);
    }

    // test frame retrieval

    for (int i = 0; i < BUCKET_COUNT; i++) {
        usize bs = block_sizes[i];
        debug_info(">>> Testing page retrieval with %u block size\n", bs);
        struct PageMeta *frame = *get_page_for_bucket(bs);
        describe("page retrieval tests",
                 assert(frame != NULL, "frame is not null"),
                 assert_equals_uint(bs, frame->block_size,
                                    "frame has correct block size"));
    }

    debug_info("Re-running with existing lists\n");

    for (int i = 0; i < BUCKET_COUNT; i++) {
        usize bs = block_sizes[i];
        debug_info(">>> Testing page retrieval with %u block size\n", bs);
        struct PageMeta *frame = *get_page_for_bucket(bs);
        describe("page retrieval tests",
                 assert(frame != NULL, "frame is not null"),
                 assert_equals_uint(bs, frame->block_size,
                                    "frame has correct block size"));
    }

    // test fixed_alloc
    for (int i = 0; i < BUCKET_COUNT; i++) {
        usize bs = block_sizes[i];
        debug_info(">>> Testing fixed_alloc for %u block size\n", bs);
        struct PageMeta *frame = *get_page_for_bucket(bs);
        usize initial_allocated = frame->total_allocated;
        void *expected_ptr = (void *)frame->next_free;
        uptr expected_head = *((uptr *)expected_ptr);
        void *ptr = fixed_alloc(bs);
        describe(
            "fixed_alloc",
            assert_equals_uint(initial_allocated + 1, frame->total_allocated,
                               "allocates a slot"),
            assert_equals_ptr(expected_ptr, ptr,
                              "allocates the next free slot"),
            assert_equals_ptr((void *)expected_head, (void *)frame->next_free,
                              "updates the free list correctly")

        );
    }

    // test filling behavior
    // a new page must be initialized because the previous tests guarantee
    // at least one full slot in each page
    for (int i = 0; i < BUCKET_COUNT; i++) {
        usize bs = block_sizes[i];
        struct PageMeta **bucket_ptr = get_page_for_bucket(bs);
        struct PageMeta *initial_frame = *bucket_ptr;
        usize block_count =  BLOCK_COUNT(**bucket_ptr);
        for (usize j = 0; j < block_count; j++) fixed_alloc(bs);
        describe("fixed_alloc page rotation",
                 assert_equals_uint(BLOCK_COUNT(*initial_frame),
                                    initial_frame->total_allocated,
                                    "the first page has all slots allocated"),
                 assert_equals_ptr(
                     NULL, (void *)initial_frame->next_free,
                     "internal free list head is NULL after page filled up"),

                 assert(initial_frame != *bucket_ptr,
                        "switched frame head to a different page"));
    }
}

#endif
