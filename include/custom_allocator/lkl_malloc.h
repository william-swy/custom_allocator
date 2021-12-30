#pragma once

#include <sys/types.h>

void* lkl_malloc(size_t size);

void* lkl_realloc(void* ptr, size_t requested_size);

void* lkl_calloc(size_t num_elem, size_t elem_size);

void lkl_free(void* ptr);