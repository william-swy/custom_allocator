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

# "library" to set compile options i.e. the c++ standard
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

# Compiler warnings
include("${CMAKE_SOURCE_DIR}/cmake/ComplierWarnings.cmake")

# "library" to use the C language warnings specified in CompilerWarnings.cmake
add_library(project_c_warnings INTERFACE)
set_project_warnings("CC" project_c_warnings)

# Include sub-projects.
add_subdirectory("src")

# Testing
option(ENABLE_TESTING "Enable Test Builds" ON)

if(ENABLE_TESTING)
  enable_language(CXX)

  # "library" to use the CXX language warnings specified in CompilerWarnings.cmake
  add_library(project_cxx_warnings INTERFACE)
  set_project_warnings("CXX" project_cxx_warnings)

  enable_testing()
  message("Building Tests.")
  add_subdirectory("test")
endif()
