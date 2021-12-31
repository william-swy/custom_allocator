# Get all CMake files
set(ALL_CMAKE_FILES
    cmake/scripts/dev-tools.cmake
    cmake/PreventInSource.cmake
    cmake/CompilerWarnings.cmake
    cmake/Conan.cmake
    src/CMakeLists.txt
    test/CMakeLists.txt
    CMakeLists.txt)

foreach(SOURCE_FILE ${ALL_CMAKE_FILES})
  list(APPEND ALL_CMAKE_SOURCE_FILES ${CMAKE_SOURCE_DIR}/${SOURCE_FILE})
endforeach()

# Adding cmake-format target
find_program(CMAKE_FORMAT "cmake-format")
if(CMAKE_FORMAT)
  message(STATUS "cmake-format found. TARGET cmake-format available.")
  add_custom_target(cmake-format COMMAND cmake-format ${ALL_CMAKE_SOURCE_FILES} -i -c
                                         ${CMAKE_SOURCE_DIR}/.cmake-format.yaml)
else()
  message(STATUS "cmake-format not found. TARGET cmake-format not available.")
endif()
