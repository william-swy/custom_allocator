# CMakeList.txt : Top-level CMake project file, do global configuration and include sub-projects here.
#
cmake_minimum_required(VERSION 3.8)

project("custom_allocator" LANGUAGES C)

include("cmake/PreventInSource.cmake")

# Include dev tools
include("cmake/scripts/dev-tools.cmake")

# Conan for dependency management
include("cmake/Conan.cmake")
run_conan()

# Compiler warnings
include("${CMAKE_SOURCE_DIR}/cmake/ComplierWarnings.cmake")

# Include sub-projects.
add_subdirectory("src")

# testing
option(ENABLE_TESTING "Enable Test Builds" ON)

if(ENABLE_TESTING)
  enable_language(CXX)
  enable_testing()
  message("Building Tests.")
  add_subdirectory("test")
endif()
