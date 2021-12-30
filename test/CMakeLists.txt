find_package(CppUTest REQUIRED)

add_executable(lkl_malloc_tests 
	"lkl_malloc_test.cpp" 
	"all_tests.cpp"
	"mock_sbrk.cpp")

# Set compiler warnings
set_project_warnings(lkl_malloc_tests)

#target_include_directories(lkl_malloc_tests PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(lkl_malloc_tests PRIVATE custom_allocator CppUTest::CppUTest CppUTest::CppUTestExt)

target_compile_features(lkl_malloc_tests PRIVATE cxx_std_20)
