// A mock for the system call sbrk.
// This allows us to control the value that it returns
// which is typically hard to do since it is controlled
// by the OS

#include <stddef.h>

extern "C" {
void* ret_val;
void* sbrk(size_t increment)
{
  (void)increment;
  return ret_val;
}
}
