// A mock for the system call sbrk.
// This allows us to control the value that it returns
// which is typically hard to do since it is controlled
// by the OS

extern "C" {
#include <stddef.h>

char* heap_base;
size_t heap_size;
size_t heap_top;  // Current size of the heap

void init_heap(char* given_heap, size_t len)
{
  heap_base = given_heap;
  heap_size = len;
  heap_top = 0;
}

void* sbrk(size_t increment)
{
  if (heap_top + increment > heap_size) {
    return (void*)-1;
  } else {
    char* allocated_address = &heap_base[heap_top];
    heap_top += increment;
    return (void*)allocated_address;
  }
}
}
