from conans import ConanFile


class CustomAllocator(ConanFile):
    name = "CustomAllocator"
    version = "0.1"
    requires = (
        "cpputest/4.0"
    )
    generators = "cmake", "gcc", "txt", "cmake_find_package", "cmake_paths"