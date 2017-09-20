//
// Created by Barbarossa on 17.09.2017.
//

#ifndef MEMORY_MANAGER_UNIT_TESTS_H
#define MEMORY_MANAGER_UNIT_TESTS_H
void _init_test(int testNum, int n, int szPage, int resultCode);
void _malloc_test(int testNum, VA *ptr, size_t szBlock, int resultCode);
void _free_test(int testNum, VA ptr, int resultCode);
void _write_test(int testNum, VA ptr, void* buffer, size_t size, int resultCode);
void _read_test(int testNum, VA ptr, void *buffer, size_t size, int resultCode);
void run_init_tests();
void run_malloc_tests();
void run_free_tests();
void run_write_tests();
void run_read_tests();
#endif //MEMORY_MANAGER_UNIT_TESTS_H
