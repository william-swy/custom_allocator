#pragma once

#include <sys/types.h>

void* lkl_malloc(size_t size);

void* realloc(void* ptr, size_t requested_size);

void* calloc(size_t num_elem, size_t elem_size);

void lkl_free(void* ptr);