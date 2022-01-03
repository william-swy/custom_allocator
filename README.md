# custom_allocator

[![codecov](https://codecov.io/gh/william-swy/custom_allocator/branch/main/graph/badge.svg?token=R5AmMXibjU)](https://codecov.io/gh/william-swy/custom_allocator)

Attempt to look at the performance of different allocation strategies

Aimed to build on top of [this work](https://github.com/mtrebi/memory-allocators). Specifically also being able to look at cache behaviour using hardware performance counters.

### Building

```shell
cmake -S . -B ./build
```
