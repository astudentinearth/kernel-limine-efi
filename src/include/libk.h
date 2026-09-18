#pragma once

#include "hardware/allocator.h"
#include <stddef.h>

#define RESULT_BAD_ALLOC 1
#define RESULT_SUCCESS 0

#define DEFINE_LINKED_LIST(T) \
    typedef struct listnode_##T { \
        T node; \
        struct listnode_##T *next; \
    } listnode_##T; \
    static inline int push_##T(listnode_##T **list, T value) { \
        listnode_##T *node = malloc(sizeof(*node)); \
        if(node == NULL) return RESULT_BAD_ALLOC; \
        node->node = value; \
        node->next = *list; \
        *list = node; return RESULT_SUCCESS; }\


