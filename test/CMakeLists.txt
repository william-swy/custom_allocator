find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

# Compiles a static lib from the Catch2 header to shorten the entire build process
add_library(catch_main STATIC "catch_main.cpp")
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_link_libraries(catch_main PRIVATE project_options project_cxx_warnings)

add_executable(lkl_malloc_tests "lkl_malloc_test.cpp" "mock_sbrk.cpp")

# Link and also set compiler warnings and compile options
target_link_libraries(
  lkl_malloc_tests
  PRIVATE custom_allocator
          catch_main
          project_cxx_warnings
          project_options)
