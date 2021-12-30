# CMakeList.txt : CMake project for custom_allocator, include source and define project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Add source to this project's executable.
add_library(custom_allocator STATIC "lkl_malloc.c")

# Set compiler warnings
target_link_libraries(custom_allocator PRIVATE project_c_warnings)

target_include_directories(custom_allocator PUBLIC $<INSTALL_INTERFACE:include>
                                                   $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)
