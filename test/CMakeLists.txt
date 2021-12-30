find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

# Compiles a static lib from the Catch2 header to shorten the entire build process
add_library(catch_main STATIC "catch_main.cpp")
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_link_libraries(catch_main PRIVATE project_options project_cxx_warnings)

add_executable(tests "lkl_malloc_test.cpp" "mock_sbrk.cpp")

target_include_directories(tests PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/include")

# Link and also set compiler warnings and compile options
target_link_libraries(tests PRIVATE catch_main project_cxx_warnings project_options)

catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)
