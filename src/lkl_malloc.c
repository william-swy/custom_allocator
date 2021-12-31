// Linked list implementation of malloc
#include "custom_allocator/lkl_malloc.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>

static void* global_base = NULL;

struct block_meta
{
  size_t block_size;
  struct block_meta* next;
  int is_free;
};

static inline struct block_meta* find_free_block(struct block_meta** last_block, size_t request_size);
static inline struct block_meta* request_space(struct block_meta* last_block, size_t request_size);
static inline struct block_meta* get_block_ptr(void* ptr);

// TODO: Thread safe version
void* lkl_malloc(size_t requested_size)
{
  struct block_meta* block_to_give;
  // TODO: Align size

  if (requested_size <= 0) {
    return NULL;
  }

  if (!global_base) {
    block_to_give = request_space(NULL, requested_size);
    if (!block_to_give) {
      return NULL;
    }
    global_base = block_to_give;
  } else {
    struct block_meta* last = (struct block_meta*)global_base;
    block_to_give = find_free_block(&last, requested_size);
    if (!block_to_give) {
      block_to_give = request_space(last, requested_size);
      if (!block_to_give) {
        return NULL;
      }
    } else {
      // TODO: split block
      block_to_give->is_free = 0;
    }
  }

  return (block_to_give + 1);
}

void* lkl_realloc(void* ptr, size_t requested_size)
{
  // NULL case: Allocate some new memory
  if (!ptr) {
    return lkl_malloc(requested_size);
  }

  // Requested size is a shrink. At the moment do nothing.
  // Potentially do a block split in the future.
  struct block_meta* curr_block_ptr = get_block_ptr(ptr);
  if (curr_block_ptr->block_size >= requested_size) {
    return ptr;
  }

  // Requested size is a expand. Allocate some new memory
  // and free up the existing memory after the copy.
  void* new_allocation = lkl_malloc(requested_size);
  if (!new_allocation) {
    // TODO: set ERRNO
    return NULL;
  }
  memcpy(new_allocation, ptr, curr_block_ptr->block_size);
  lkl_free(ptr);
  return new_allocation;
}

void* lkl_calloc(size_t num_elem, size_t elem_size)
{
  size_t total_size = num_elem * elem_size;  // Potential overflow if product is too large
  void* new_allocation = lkl_malloc(total_size);
  memset(new_allocation, 0, total_size);
  return new_allocation;
}

void lkl_free(void* ptr)
{
  if (!ptr) {
    return;
  }

  // TODO: merge blocks
  struct block_meta* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->is_free == 0);
  block_ptr->is_free = 1;
}

struct block_meta* find_free_block(struct block_meta** last_block, size_t request_size)
{
  struct block_meta* current = (struct block_meta*)global_base;
  while (current && !(current->is_free && current->block_size >= request_size)) {
    *last_block = current;
    current = current->next;
  }
  return current;
}

struct block_meta* request_space(struct block_meta* last_block, size_t request_size)
{
  struct block_meta* requested_block = (struct block_meta*)sbrk(0);
  void* requested_alloc = (struct block_meta*)sbrk((intptr_t)(request_size + sizeof(struct block_meta)));

  if (requested_alloc == (void*)-1) {
    return NULL;
  }

  assert(requested_alloc == requested_block);

  if (last_block) {
    last_block->next = requested_block;
  }

  requested_block->block_size = request_size;
  requested_block->next = NULL;
  requested_block->is_free = 0;

  return requested_block;
}

struct block_meta* get_block_ptr(void* ptr)
{
  return ((struct block_meta*)ptr - 1);
}