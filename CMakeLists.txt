cmake_minimum_required(VERSION 3.8)
project(memory_manager)

set(CMAKE_C_STANDARD 99)
set(SOURCE_FILES main.c mmemory.h paging.h paging.c tests/load_tests.c tests/load_tests.h tests/unit_tests/malloc_tests.c tests/unit_tests/malloc_tests.h tests/unit_tests/init_tests.c tests/unit_tests/init_tests.h tests/unit_tests/free_tests.c tests/unit_tests/free_tests.h)
add_executable(memory_manager ${SOURCE_FILES} tests/unit_tests/malloc_tests.c tests/unit_tests/malloc_tests.h tests/unit_tests/init_tests.c)