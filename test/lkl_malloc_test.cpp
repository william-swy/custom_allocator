// Unit tests for linked list malloc implementation.
// Since lkl_malloc.c uses a system call i.e. sbrk, the system call is mocked
// in mock_sbrk.cpp by relying on the fact that system symbols are weak symbols
// or what is sometimes called a "link seam".
//
// See the following references:
// https://stackoverflow.com/questions/44073243/how-to-mock-socket-in-c
// https://stackoverflow.com/questions/2924440/advice-on-mocking-system-calls

#include <algorithm>
#include <array>
#include <catch2/catch.hpp>
#include <cstddef>
#include <random>

extern "C" {
#include "lkl_malloc.c"
extern void init_heap(char* given_heap, size_t len);
}

// Checks if the returned pointer to newly allocated memory is within the bounds of the specified heap
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

TEST_CASE("lkl_malloc repeat allocate until full constant size", "[lkl_malloc]")
{
  global_base = NULL;
  REQUIRE(global_base == NULL);

  SECTION("Total allocations uses heap completely - no fragmentation")
  {
    constexpr std::size_t request_size = 8;
    constexpr std::size_t single_actual_alloc_size = request_size + sizeof(struct block_meta);
    constexpr std::size_t heap_size = 4 * single_actual_alloc_size;

    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    // Should be able to request 4 times
    // Request 1
    char* req1 = reinterpret_cast<char*>(lkl_malloc(request_size));
    REQUIRE(req1 != NULL);
    REQUIRE(req1 == test_heap + sizeof(struct block_meta));
    REQUIRE(ptr_in_bounds(req1, request_size, test_heap, heap_size));

    // Request 2
    char* req2 = reinterpret_cast<char*>(lkl_malloc(request_size));
    REQUIRE(req2 != NULL);
    REQUIRE(req2 == req1 + single_actual_alloc_size);
    REQUIRE(ptr_in_bounds(req2, request_size, test_heap, heap_size));

    // Request 3
    char* req3 = reinterpret_cast<char*>(lkl_malloc(request_size));
    REQUIRE(req3 != NULL);
    REQUIRE(req3 == req2 + single_actual_alloc_size);
    REQUIRE(ptr_in_bounds(req3, request_size, test_heap, heap_size));

    // Request 4
    char* req4 = reinterpret_cast<char*>(lkl_malloc(request_size));
    REQUIRE(req4 != NULL);
    REQUIRE(req4 == req3 + single_actual_alloc_size);
    REQUIRE(ptr_in_bounds(req4, request_size, test_heap, heap_size));

    // Request 5
    void* req5 = lkl_malloc(request_size);
    REQUIRE(req5 == NULL);
  }

  SECTION("Total allocations uses heap partially - fragmentation")
  {
    constexpr std::size_t request_size = 16;
    constexpr std::size_t single_actual_alloc_size = request_size + sizeof(struct block_meta);
    constexpr std::size_t heap_size = 3 * single_actual_alloc_size + 8;

    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    // Should be able to request space 3 times

    // Request 1
    char* req1 = reinterpret_cast<char*>(lkl_malloc(request_size));
    REQUIRE(req1 != NULL);
    REQUIRE(req1 == test_heap + sizeof(struct block_meta));
    REQUIRE(ptr_in_bounds(req1, request_size, test_heap, heap_size));

    // Request 2
    char* req2 = reinterpret_cast<char*>(lkl_malloc(request_size));
    REQUIRE(req2 != NULL);
    REQUIRE(req2 == req1 + single_actual_alloc_size);
    REQUIRE(ptr_in_bounds(req2, request_size, test_heap, heap_size));

    // Request 3
    char* req3 = reinterpret_cast<char*>(lkl_malloc(request_size));
    REQUIRE(req3 != NULL);
    REQUIRE(req3 == req2 + single_actual_alloc_size);
    REQUIRE(ptr_in_bounds(req3, request_size, test_heap, heap_size));

    // Request 4 No more room
    void* req4 = lkl_malloc(request_size);
    REQUIRE(req4 == NULL);
  }
}

TEST_CASE("lkl_malloc repeat allocate until full variable size", "[lkl_malloc]")
{
  global_base = NULL;
  REQUIRE(global_base == NULL);

  constexpr std::size_t req_size8 = 8;
  constexpr std::size_t req_size16 = 16;
  constexpr std::size_t req_size24 = 24;

  constexpr std::size_t req_size8_act = req_size8 + sizeof(struct block_meta);
  constexpr std::size_t req_size16_act = req_size16 + sizeof(struct block_meta);
  constexpr std::size_t req_size24_act = req_size24 + sizeof(struct block_meta);

  SECTION("Total allocations uses heap completely - no fragmentation")
  {
    constexpr std::size_t heap_size = req_size8_act + req_size16_act + req_size24_act + req_size16_act;
    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    // Should be able to request 4 times in the order of 8, 16, 24, 16

    char* req1 = reinterpret_cast<char*>(lkl_malloc(req_size8));
    REQUIRE(req1 != NULL);
    REQUIRE(req1 == test_heap + sizeof(struct block_meta));
    REQUIRE(ptr_in_bounds(req1, req_size8, test_heap, heap_size));

    char* req2 = reinterpret_cast<char*>(lkl_malloc(req_size16));
    REQUIRE(req2 != NULL);
    REQUIRE(req2 == req1 + req_size8_act);
    REQUIRE(ptr_in_bounds(req2, req_size16, test_heap, heap_size));

    char* req3 = reinterpret_cast<char*>(lkl_malloc(req_size24));
    REQUIRE(req3 != NULL);
    REQUIRE(req3 == req2 + req_size16_act);
    REQUIRE(ptr_in_bounds(req3, req_size24, test_heap, heap_size));

    char* req4 = reinterpret_cast<char*>(lkl_malloc(req_size16));
    REQUIRE(req4 != NULL);
    REQUIRE(req4 == req3 + req_size24_act);
    REQUIRE(ptr_in_bounds(req4, req_size16, test_heap, heap_size));

    char* req5 = reinterpret_cast<char*>(lkl_malloc(req_size8));
    REQUIRE(req5 == NULL);
  }

  SECTION("Total allocations uses heap partially - fragmentation")
  {
    constexpr std::size_t heap_size = req_size16_act + req_size24_act + req_size16_act + req_size8_act;
    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    // Can allocate 16, 24, 16 but next 16 cannot allocate
    char* req1 = reinterpret_cast<char*>(lkl_malloc(req_size16));
    REQUIRE(req1 != NULL);
    REQUIRE(req1 == test_heap + sizeof(struct block_meta));
    REQUIRE(ptr_in_bounds(req1, req_size16, test_heap, heap_size));

    char* req2 = reinterpret_cast<char*>(lkl_malloc(req_size24));
    REQUIRE(req2 != NULL);
    REQUIRE(req2 == req1 + req_size16_act);
    REQUIRE(ptr_in_bounds(req2, req_size24, test_heap, heap_size));

    char* req3 = reinterpret_cast<char*>(lkl_malloc(req_size16));
    REQUIRE(req3 != NULL);
    REQUIRE(req3 == req2 + req_size24_act);
    REQUIRE(ptr_in_bounds(req3, req_size16, test_heap, heap_size));

    char* req4 = reinterpret_cast<char*>(lkl_malloc(req_size16));
    REQUIRE(req4 == NULL);
  }
}

TEST_CASE("lkl_malloc reuses freed blocks", "[lkl_malloc]")
{
  global_base = NULL;
  REQUIRE(global_base == NULL);

  constexpr std::size_t heap_size = 0x100;
  char test_heap[heap_size];
  init_heap(test_heap, heap_size);

  SECTION("reuse previously freed block")
  {
    constexpr std::size_t request_size = 8;
    void* init_alloc = lkl_malloc(request_size);  // technically dangling but we control how memory is allocated so we can deem this non UB.
    lkl_free(init_alloc);
    void* next_alloc = lkl_malloc(request_size);
    REQUIRE(next_alloc != NULL);
    REQUIRE(ptr_in_bounds(reinterpret_cast<char*>(next_alloc), request_size, test_heap, heap_size));
    REQUIRE(next_alloc == init_alloc);
  }

  SECTION("reuse first available freed block")
  {
    constexpr std::size_t req_size = 8;
    void* fst_alloc = lkl_malloc(req_size);
    void* sec_alloc = lkl_malloc(req_size);
    lkl_free(sec_alloc);
    void* trd_alloc = lkl_malloc(req_size);

    REQUIRE(trd_alloc != NULL);
    REQUIRE(ptr_in_bounds(reinterpret_cast<char*>(trd_alloc), req_size, test_heap, heap_size));
    REQUIRE(trd_alloc == sec_alloc);
  }

  SECTION("reuse first available freed block of suffice size")
  {
    constexpr std::size_t num_allocs = 5;
    constexpr std::array<std::size_t, num_allocs> alloc_sizes = {8, 16, 8, 32, 64};
    std::array<void*, num_allocs> alloc_ptrs = {0, 0, 0, 0, 0};

    for (std::size_t idx = 0; idx < num_allocs; idx++) {
      alloc_ptrs[idx] = lkl_malloc(alloc_sizes[idx]);
    }

    std::for_each(alloc_ptrs.begin(), alloc_ptrs.end(), lkl_free);

    constexpr std::size_t req_size = 24;
    void* new_alloc = lkl_malloc(req_size);
    REQUIRE(new_alloc != NULL);
    REQUIRE(ptr_in_bounds(reinterpret_cast<char*>(new_alloc), req_size, test_heap, heap_size));
    REQUIRE(new_alloc == alloc_ptrs[3]);
  }

  SECTION("multiple reuse")
  {
    std::size_t req_size = 64;
    void* fst_alloc = lkl_malloc(req_size);
    void* sec_alloc = lkl_malloc(req_size);

    lkl_free(fst_alloc);
    lkl_free(sec_alloc);

    void* fst_reuse = lkl_malloc(req_size);
    void* sec_reuse = lkl_malloc(req_size);

    REQUIRE(fst_reuse != NULL);
    REQUIRE(ptr_in_bounds(reinterpret_cast<char*>(fst_reuse), req_size, test_heap, heap_size));
    REQUIRE(fst_reuse == fst_alloc);

    REQUIRE(sec_reuse != NULL);
    REQUIRE(ptr_in_bounds(reinterpret_cast<char*>(sec_reuse), req_size, test_heap, heap_size));
    REQUIRE(sec_reuse == sec_alloc);
  }
}

TEST_CASE("lkl_malloc various workloads", "[lkl_malloc]")
{
  global_base = NULL;
  REQUIRE(global_base == NULL);

  SECTION("Multiple repeat fixed-size allocation then repeat free")
  {
    constexpr std::size_t alloc_size = 4096 - sizeof(struct block_meta);
    constexpr std::size_t num_allocs = 1000;
    constexpr std::size_t heap_size = (alloc_size + sizeof(struct block_meta)) * num_allocs;
    constexpr std::size_t num_iter = 1000;

    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    std::array<void*, num_allocs> alloc_ptrs;
    alloc_ptrs.fill(NULL);

    for (std::size_t iter = 0; iter < num_iter; iter++) {
      for (std::size_t alloc_num = 0; alloc_num < num_allocs; alloc_num++) {
        if (alloc_ptrs[alloc_num] != NULL) {
          lkl_free(alloc_ptrs[alloc_num]);
          alloc_ptrs[alloc_num] = NULL;
        } else {
          void* alloc_res = lkl_malloc(alloc_size);

          REQUIRE(alloc_res != NULL);
          REQUIRE(ptr_in_bounds(reinterpret_cast<char*>(alloc_res), alloc_size, test_heap, heap_size));

          alloc_ptrs[alloc_num] = alloc_res;
        }
      }
    }
  }

  SECTION("Multiple random allocations and frees from uniform distribution")
  {
    constexpr std::size_t min_alloc_size = 8;
    constexpr std::size_t max_alloc_size = 4096;
    constexpr std::size_t num_rand_allocs = 1024;
    constexpr std::size_t num_rand_iters = 1000000;

    constexpr std::size_t heap_size = num_rand_allocs * (max_alloc_size + sizeof(struct block_meta));
    char test_heap[heap_size];
    init_heap(test_heap, heap_size);

    std::string seed_str("3458755949");  // Some random string
    std::seed_seq seed(seed_str.begin(), seed_str.end());
    std::mt19937 gen(seed);  // mersenne twister

    std::uniform_int_distribution<std::size_t> alloc_size_rng(min_alloc_size, max_alloc_size);
    std::uniform_int_distribution<std::size_t> idx_access_rng(0, num_rand_allocs - 1);

    std::array<std::size_t, num_rand_allocs> alloc_sizes;
    for (std::size_t idx = 0; idx < num_rand_allocs; idx++) {
      alloc_sizes[idx] = alloc_size_rng(gen);
    }

    std::array<void*, num_rand_allocs> alloc_ptrs;
    alloc_ptrs.fill(NULL);

    for (std::size_t iter = 0; iter < num_rand_iters; iter++) {
      std::size_t idx = idx_access_rng(gen);
      if (alloc_ptrs[idx] != NULL) {
        lkl_free(alloc_ptrs[idx]);
        alloc_ptrs[idx] = NULL;
      } else {
        void* res = lkl_malloc(alloc_sizes[idx]);
        REQUIRE(res != NULL);
        REQUIRE(ptr_in_bounds(reinterpret_cast<char*>(res), alloc_sizes[idx], test_heap, heap_size));
        alloc_ptrs[idx] = res;
      }
    }
  }
}