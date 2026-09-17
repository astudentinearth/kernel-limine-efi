
#include "test/test.h"
#include "debug.h"
#include "test/assert.h"

void run_tests()
{
#ifdef TEST_MODE
    test_memcpy();
    test_string_h();
    test_allocator();
    test_paging();
    test_malloc();
    test_vmm();
    print_test_report();
    debug_printf("[DEBUG] CR3=%p | CR4=%p\n", get_cr3(), get_cr4());
#endif
}
