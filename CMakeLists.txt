cmake_minimum_required(VERSION 3.8)
project(memory_manager)

set(CMAKE_C_STANDARD 99)
set(SOURCE_FILES main.c mmemory.h tests/unit_tests.c paging.h paging.c tests/unit_tests.h tests/load_tests.c tests/load_tests.h)
add_executable(memory_manager ${SOURCE_FILES})