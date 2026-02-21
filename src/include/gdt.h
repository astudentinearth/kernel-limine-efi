#pragma once
#include <stdint.h>
void setup_gdt();


// base_8 flags_4 limit_4 access_byte_8 base_8 base_16 limit_16
#define NULL_DESCRIPTOR     0x0000000000000000
#define KERNEL_CS           0x00AF9A000000FFFF
#define KERNEL_DATA         0x00CF92000000FFFF
#define USER_DATA           0x00CFF2000000FFFF
#define USER_CODE           0x00AFFA000000FFFF

#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define RSP0_SIZE 16384
#define IST1_SIZE 4096

struct system_segment_descriptor {
    uint32_t reserved;
    uint32_t base_63_32;
    uint8_t base_31_24;
    uint8_t flags_limit;
    uint8_t access_byte;
    uint8_t base_23_16;
    uint8_t base_15_0;
    uint8_t limit;
}__attribute__((packed));

struct TSS {
    uint32_t reserved_0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved_1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved_2;
    uint16_t reserved_3;
    uint16_t iopb;
}__attribute__((packed));

struct GDT {
    uint64_t null_descriptor;
    uint64_t kernel_cs;
    uint64_t kernel_data;
    uint64_t user_data;
    uint64_t user_code;
    struct system_segment_descriptor ssd;
};

