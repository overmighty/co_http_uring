# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: Apache-2.0

find_package(GTest REQUIRED)

enable_testing()
include(GoogleTest)

add_executable(co_http_uring_test server_test.cpp)

target_compile_features(co_http_uring PUBLIC cxx_std_20)
target_compile_options(
    co_http_uring_test PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:-Wall -Wextra -pedantic>
)

target_link_libraries(co_http_uring_test co_http_uring GTest::gtest_main)

gtest_discover_tests(co_http_uring_test)
