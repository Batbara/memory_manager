cmake_minimum_required(VERSION 3.8)
project(memory_manager)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c mmemory.h unit_tests.c paging.h paging.c)
add_executable(memory_manager ${SOURCE_FILES})