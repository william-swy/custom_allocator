// Unit tests for linked list malloc implementation.
// Since lkl_malloc.c uses a system call i.e. sbrk, the system call is mocked
// in mock_sbrk.cpp by relying on the fact that system symbols are weak symbols
// or what is sometimes called a "link seam".
//
// See the following references:
// https://stackoverflow.com/questions/44073243/how-to-mock-socket-in-c
// https://stackoverflow.com/questions/2924440/advice-on-mocking-system-calls

#include <catch2/catch.hpp>

extern "C" {
#include "custom_allocator/lkl_malloc.h"
extern void* ret_val;
}

TEST_CASE("First allocation - allocate zero space", "[lkl_malloc]")
{
  REQUIRE(lkl_malloc(0) == NULL);
}

TEST_CASE("First allocation - no space available", "[lkl_malloc]")
{
  ret_val = reinterpret_cast<void*>(-1);
  constexpr size_t request_size = 100;
  REQUIRE(lkl_malloc(request_size) == NULL);
}
