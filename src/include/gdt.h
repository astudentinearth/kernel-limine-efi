#pragma once
#include "stdint.h"
void setup_gdt();

// base_8 flags_4 limit_4 access_byte_8 base_8 base_16 limit_16
#define NULL_DESCRIPTOR 0x0000000000000000
#define KERNEL_CS 0x00AF9A000000FFFF
#define KERNEL_DATA 0x00CF92000000FFFF
#define USER_DATA 0x00CFF2000000FFFF
#define USER_CODE 0x00AFFA000000FFFF

#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define RSP0_SIZE 16384
#define IST1_SIZE 4096

struct system_segment_descriptor {
    u32 reserved;
    u32 base_63_32;
    u8 base_31_24;
    u8 flags_limit;
    u8 access_byte;
    u8 base_23_16;
    u8 base_15_0;
    u8 limit;
} __attribute__((packed));

struct TSS {
    u32 reserved_0;
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;
    u64 reserved_1;
    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;
    u64 reserved_2;
    u16 reserved_3;
    u16 iopb;
} __attribute__((packed));

struct GDT {
    u64 null_descriptor;
    u64 kernel_cs;
    u64 kernel_data;
    u64 user_data;
    u64 user_code;
    struct system_segment_descriptor ssd;
};
