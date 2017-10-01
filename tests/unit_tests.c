#include <stdio.h>
#include "../mmemory.h"
#include "../paging.h"
#include <malloc.h>
#include <mem.h>

void _init_test(int testNum, int n, size_t szPage, int resultCode) {
    printf("\tTest %d: ", testNum);
    int returnValue = _init(n, szPage);
    if (returnValue == resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}

void _malloc_test(int testNum, VA *ptr, size_t szBlock, int resultCode) {
    printf("\tTest %d: ", testNum);
    int returnValue = _malloc(ptr, szBlock);
    if (returnValue == resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}

void _free_test(int testNum, VA ptr, int resultCode) {
    printf("\tTest %d: ", testNum);
    int returnValue = _free(ptr);
    if (returnValue == resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}

void _write_test(int testNum, VA ptr, void *buffer, size_t size, int resultCode) {
    printf("\tTest %d: ", testNum);
    int returnValue = _write(ptr, buffer, size);
//    printf("res is %d; ",returnValue);
    if (returnValue == resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}

void _read_test(int testNum, VA ptr, void *buffer, size_t size, int resultCode) {
    printf("\tTest %d: ", testNum);
    int returnValue = _read(ptr, buffer, size);
    if (returnValue == resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}

void freeGlobalVars() {
    free(pool);
    free(table);
    free(virtualPages);
}
void printBlockUsage(){
    struct block *current = pool;
    printf("blocks availability:\n");
    while(current->next!=NULL){
        printf("%c\n",current->isUsed);
        current=current->next;
    }
}

void run_init_tests() {
    printf("\n--------_init tests--------\n");
    _init_test(0, 5, 32, SUCCESS);
    freeGlobalVars();

    _init_test(1, -2, 64, WRONG_ARGUMENTS);
    freeGlobalVars();

    _init_test(2, 10, 15, WRONG_ARGUMENTS);
    freeGlobalVars();

    _init_test(3, 20, 128, SUCCESS);
    freeGlobalVars();

    _init_test(4, 100, 1025, WRONG_ARGUMENTS);
    freeGlobalVars();
}

void run_malloc_tests() {
    // начальные условия: 10 стр, размер 4
    _init(10, 4);
    printf("\n--------_malloc tests--------\n");
    VA addr = "00000000000000000";
    _malloc_test(0, &addr, 16, SUCCESS);
    addr = "0065465465";
    _malloc_test(1, &addr, 4, WRONG_ARGUMENTS);

    addr = "00000000000011110";
    _malloc_test(2, &addr, 16, MEMORY_LACK);

    addr = "00000000000000001";
    _malloc_test(3, &addr, 16, UNKNOWN_MISTAKE); //попытка выделить уже занятый адрес

//    printBlockUsage();
//    for(int count = 0; count<25; count++) {
//        _free(convertToVA(count));
//    }
//    printBlockUsage();
//    freeGlobalVars();
//    _init(10, 4);
    addr = "00000000000010010"; //18
    _malloc_test(4, &addr, 5, SUCCESS);

    freeGlobalVars();
}

void run_free_tests() {
//    freeAll();
    // freeGlobalVars();
    //начальные условия
    printf("\n--------_free tests--------\n");
    _init(10, 4);
    VA addr = "00000000000000000";
    _malloc(&addr, 16);

    _free_test(0, addr, SUCCESS);
    _free_test(1, addr, UNKNOWN_MISTAKE);

    VA incorrectAddr = "00lol0kek00000";
    _free_test(2, incorrectAddr, WRONG_ARGUMENTS);

    addr = "00000000000000100";
    _free_test(3, addr, SUCCESS);

    addr = "00000000000011000";
    _free_test(4, addr, UNKNOWN_MISTAKE);
    freeGlobalVars();
}

void run_write_tests() {
    printf("\n--------_write tests--------\n");
    _init(10, 4);
    VA addr = "00000000000000000";
    _malloc(&addr, 16);

    _write_test(0, addr, "os", 2, SUCCESS);
    _write_test(1, addr, "os", 2, UNKNOWN_MISTAKE);
    _write_test(2, addr, "lol", 3, MEMORY_LACK);

    addr = "00000000000001000";
    _write_test(3, addr, "wp", 2, SUCCESS);

    VA faultyAddr = "lolkekcheburek";
    _write_test(4, faultyAddr, "tt", 2, WRONG_ARGUMENTS);
    freeGlobalVars();
}

void run_read_tests() {
    printf("\n--------_read tests--------\n");
    _init(10, 4);
    VA addr = "00000000000000000";
    _malloc(&addr, 16);
    _write(addr, "os", 2);
    char *ptr = calloc(2, 2 * sizeof(char));

    _read_test(0, addr, ptr, 2, SUCCESS);
    _read_test(1, addr, ptr, 4, MEMORY_LACK);

    VA faultyAddr = "it is not an address!!";
    _read_test(2, faultyAddr, ptr, 2, WRONG_ARGUMENTS);

    addr = "00000000000001000";
    _write(addr, "c4", 2);
    _read_test(3, addr, ptr, 2, SUCCESS);

    addr = "00000000000011000";
    _read_test(4, addr, ptr, 2, UNKNOWN_MISTAKE);
    freeGlobalVars();
}

