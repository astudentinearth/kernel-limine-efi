#pragma once
#include "stdint.h"
#include <stdbool.h>
bool cpu_has_msr();
void cpu_get_msr(u32 msr, u32 *lo, u32 *hi);
void cpu_set_msr(u32 msr, u32 lo, u32 hi);
