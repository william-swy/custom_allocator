# custom_allocator

[![codecov](https://codecov.io/gh/william-swy/custom_allocator/branch/main/graph/badge.svg?token=R5AmMXibjU)](https://codecov.io/gh/william-swy/custom_allocator)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/william-swy/custom_allocator.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/william-swy/custom_allocator/context:cpp)

Attempt to look at the performance of different allocation strategies

Aimed to build on top of [this work](https://github.com/mtrebi/memory-allocators). Specifically also being able to look at cache behaviour using hardware performance counters.

### Building

```shell
cmake -S . -B ./build
```
