# Copyright (C) 2022 OverMighty
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

project(co_http_uring VERSION 0.1.0 LANGUAGES CXX)

add_library(
    co_http_uring
    src/connection.cpp
    src/connection_reader.cpp
    src/connection_writer.cpp
    src/eventfd.cpp
    src/http_utils.cpp
    src/io_uring.cpp
    src/request.cpp
    src/response_writer.cpp
    src/server.cpp
    src/status_code.cpp
    src/tcp_socket.cpp
    include/co_http_uring/version.hpp.in)

option(ENABLE_SANITIZERS "Enable sanitizers.")

if(ENABLE_SANITIZERS)
    set(
        SANITIZER_OPTIONS
        $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:-fsanitize=address,undefined>
    )
    target_compile_options(co_http_uring BEFORE PUBLIC ${SANITIZER_OPTIONS})
    target_link_options(co_http_uring BEFORE PUBLIC ${SANITIZER_OPTIONS})
endif()

target_compile_features(co_http_uring PUBLIC cxx_std_20)
target_compile_options(
    co_http_uring PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:-Wall -Wextra -pedantic>
)

configure_file(
    include/co_http_uring/version.hpp.in
    include/co_http_uring/version.hpp
)
target_sources(
    co_http_uring PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}/include
    FILES
    include/co_http_uring/connection.hpp
    include/co_http_uring/connection_future.hpp
    include/co_http_uring/connection_reader.hpp
    include/co_http_uring/connection_writer.hpp
    include/co_http_uring/error.hpp
    include/co_http_uring/eventfd.hpp
    include/co_http_uring/headers.hpp
    include/co_http_uring/http_utils.hpp
    include/co_http_uring/io_uring.hpp
    include/co_http_uring/request.hpp
    include/co_http_uring/response_writer.hpp
    include/co_http_uring/server.hpp
    include/co_http_uring/socket_address.hpp
    include/co_http_uring/status_code.hpp
    include/co_http_uring/task.hpp
    include/co_http_uring/tcp_socket.hpp
    include/co_http_uring/types.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/co_http_uring/version.hpp
)

find_package(PkgConfig REQUIRED)

find_package(fmt REQUIRED)
target_link_libraries(co_http_uring PUBLIC fmt::fmt)

pkg_check_modules(LIBURING REQUIRED liburing)
target_include_directories(co_http_uring PRIVATE ${LIBURING_INCLUDE_DIRS})
target_compile_options(co_http_uring PRIVATE ${LIBURING_CFLAGS_OTHER})
target_link_directories(co_http_uring PRIVATE ${LIBURING_LIBRARY_DIRS})
target_link_libraries(co_http_uring PRIVATE ${LIBURING_LIBRARIES})
target_link_options(co_http_uring PRIVATE ${LIBURING_LDFLAGS_OTHER})

set_target_properties(
    co_http_uring
    PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}
)

install(
    TARGETS co_http_uring
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

add_subdirectory(test)
