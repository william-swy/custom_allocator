// Unit tests for linked list malloc implementation.
// Since lkl_malloc.c uses a system call i.e. sbrk, the system call is mocked
// in mock_sbrk.cpp by relying on the fact that system symbols are weak symbols
// or what is sometimes called a "link seam".
//
// See the following references:
// https://stackoverflow.com/questions/44073243/how-to-mock-socket-in-c
// https://stackoverflow.com/questions/2924440/advice-on-mocking-system-calls

#include <catch2/catch.hpp>
#include <cstddef>

extern "C" {
#include "lkl_malloc.c"
extern void init_heap(char* given_heap, size_t len);
}

bool ptr_in_bounds(const char* ptr, std::size_t alloc_size, const char* heap_start, std::size_t heap_size)
{
  return !(ptr < heap_start || ptr + alloc_size > heap_start + heap_size);
}

TEST_CASE("lkl_malloc first allocation", "[lkl_malloc]")
{
  global_base = NULL;
  REQUIRE(global_base == NULL);

  SECTION("allocate zero space")
  {
    constexpr std::size_t heap_size = 16;
    constexpr std::size_t request_size = 0;
    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    REQUIRE(lkl_malloc(request_size) == NULL);
  }

  SECTION("no space available")
  {
    constexpr std::size_t heap_size = 16;
    constexpr std::size_t request_size = 32;
    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    REQUIRE(lkl_malloc(request_size) == NULL);
  }

  SECTION("space available")
  {
    constexpr std::size_t heap_size = 64;
    constexpr std::size_t request_size = 8;
    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    void* request_res = lkl_malloc(request_size);

    REQUIRE(request_res != NULL);

    char* res_heap_ptr = reinterpret_cast<char*>(request_res);
    REQUIRE(res_heap_ptr == test_heap + sizeof(struct block_meta));

    REQUIRE(ptr_in_bounds(res_heap_ptr, request_size, test_heap, heap_size));
  }
}
