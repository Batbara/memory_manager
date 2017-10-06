#include <stdio.h>
#include "../mmemory.h"
#include "../paging.h"
#include <malloc.h>
#include <mem.h>
#include <assert.h>

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
    assert(_init(5,32)==SUCCESS);
    assert(_init(-2,64)==WRONG_ARGUMENTS);
    assert(_init(10,15)==WRONG_ARGUMENTS);
    assert(_init(20,128)==SUCCESS);
    assert(_init(100,1025)==WRONG_ARGUMENTS);
}
void custom_test(){
    _init(10,4);
    for(int i = 0; i<20; i+=1){
        VA ptr = convertToVA(i);
        _malloc(&ptr,2);
    }
    for(int i = 5; i<19; i+=1){
        VA ptr = convertToVA(i);
        _write(ptr,"pp",2);
    }
    printf("\nread result:\t");
    for(int i = 7; i<18; i++){
        VA ptr = convertToVA(i);
        char *buff = calloc(sizeof(char),sizeof(char));

        _read(ptr,buff,1);
        printf("%s",buff);
    }
}
void run_malloc_tests() {
    _init(30,8);
    printf("\n--------_malloc tests--------\n");
    VA addr = "0000000000011000";
    assert(_malloc(&addr,8)==SUCCESS);

    addr = "0065465465";
    assert(_malloc(&addr,4)==WRONG_ARGUMENTS);

    addr = "0000000000011110";
    assert(_malloc(&addr,250)==MEMORY_LACK);

    addr = "0000000000011000";
   //  попытка выделить уже занятый адрес
    assert(_malloc(&addr,2)==UNKNOWN_MISTAKE);

    addr = "0000000000010010"; //18
    assert(_malloc(&addr,5)==SUCCESS);
}

void run_free_tests() {
    //начальные условия
    printf("\n--------_free tests--------\n");
    _init(10, 4);
    VA addr = "0000000000000000";
    _malloc(&addr, 16);

    assert(_free(addr)==SUCCESS);
    assert(_free(addr)==UNKNOWN_MISTAKE);

    VA incorrectAddr = "00lol0kek00000";
    assert(_free(incorrectAddr)==WRONG_ARGUMENTS);

    addr = "0000000000000100";
    assert(_free(addr)==SUCCESS);

    addr = "0000000000011000";
    assert(_free(addr)==UNKNOWN_MISTAKE);
}

void run_write_tests() {
    printf("\n--------_write tests--------\n");
    _init(10, 4);
    VA addr = "0000000000000000";
    _malloc(&addr, 16);

    assert(_write(addr,"os",2)==SUCCESS);
    assert(_write(addr,"os",2)==UNKNOWN_MISTAKE);
    assert(_write(addr,"os",3)==MEMORY_LACK);
    assert(_write(addr,"lol",3)==MEMORY_LACK);

    addr = "0000000000001000";
    assert(_write(addr,"wp",2)==SUCCESS);

    VA faultyAddr = "lolkekcheburek";
    assert(_write(faultyAddr,"tt",2)==WRONG_ARGUMENTS);
}

void run_read_tests() {
    printf("\n--------_read tests--------\n");
    _init(10, 4);
    VA addr = "0000000000000000";
    _malloc(&addr, 16);
    _write(addr, "os", 2);
    char *ptr = calloc(2, 2 * sizeof(char));

    assert(_read(addr,ptr,2)==SUCCESS);
    assert(_read(addr, ptr, 4)==MEMORY_LACK);

    VA faultyAddr = "it is not an address!!";
    assert(_read(faultyAddr, ptr, 2)==WRONG_ARGUMENTS);

    addr = "0000000000001000";
    _write(addr, "c4", 2);
    assert(_read(addr,ptr,2)==SUCCESS);

    addr = "0000000000011000";
    assert(_read(addr,ptr,2)==UNKNOWN_MISTAKE);
}

