
#include <stdint.h>
uint64_t get_usable_memory();
void init_memory_map();

#ifdef TEST_MODE
void dump_memory_info();
#endif

