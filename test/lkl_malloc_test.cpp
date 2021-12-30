// Unit tests for linked list malloc implementation.
// Since lkl_malloc.c uses a system call i.e. sbrk, the system call is mocked
// in mock_sbrk.cpp by relying on the fact that system symbols are weak symbols
// or what is sometimes called a "link seam".
//
// See the following references:
// https://stackoverflow.com/questions/44073243/how-to-mock-socket-in-c
// https://stackoverflow.com/questions/2924440/advice-on-mocking-system-calls

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "custom_allocator/lkl_malloc.h"
extern void* ret_val;
}

TEST_GROUP(lkl_malloc_test){

};

TEST(lkl_malloc_test, AllocateZero)
{
  CHECK(lkl_malloc(0) == NULL);
}

TEST(lkl_malloc_test, FirstAllocationNoAvailableSpace)
{
  ret_val = reinterpret_cast<void*>(-1);
  constexpr size_t request_size = 100;
  CHECK(lkl_malloc(request_size) == NULL);
}