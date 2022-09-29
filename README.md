# co_http_uring

> C++20 HTTP/1.1 server library using coroutines and Linux's io_uring API.

## Introduction

co_http_uring is a C++20 HTTP/1.1 library developed to experiment with
[coroutines](https://en.cppreference.com/w/cpp/language/coroutines) and Linux's
[io_uring API](https://unixism.net/loti/what_is_io_uring.html) through
[liburing](https://github.com/axboe/liburing).

## Getting Started

### Prerequisites

- Linux
- [CMake](https://cmake.org/) >= 3.23
- a C++20 compiler toolchain supported by CMake
- **libraries:**
  - [{fmt}](https://fmt.dev/latest/index.html)
  - [liburing](https://github.com/axboe/liburing)
- **test libraries:**
  - [GoogleTest](https://github.com/google/googletest)

### Building

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ..
    $ cmake --build .

### Installation

    # cmake --install .

## License

This software is licensed under the [Apache License, version 2.0](
./LICENSE.txt).
