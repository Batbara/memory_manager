#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <>
#include "../../mmemory.h"
#include "../../paging.h"

void read_11_from_14_written_successReturned() {
    _init(10, 4);
    VA addr;
    _malloc(&addr, 10);
    _malloc(&addr, 20);
    char *buff = calloc(11 * sizeof(char), 11 * sizeof(char));

    _write(addr+2, "teststring1234", 14);

    assert(_read(addr+4, buff, 11) == SUCCESS);
    printf("%s\n", buff);

    free(buff);
    freeAll();

}

void read_5_from_14_written_with_offset_successReturned() {
    _init(20, 4);
    VA addr;
    _malloc(&addr, 80);

    char *buff = calloc(5 * sizeof(char), 5 * sizeof(char));
    _write(addr, "teststring1234", 14);
    addr += 4;
    assert(_read(addr, buff, 5) == SUCCESS);

    printf("%s\n", buff);

    free(buff);
    freeAll();
}

void read_3_with_4_written_successReturned() {
    _init(12, 8);

    VA addr;
    _malloc(&addr, 20);

    char *buff = calloc(3 * sizeof(char), 3 * sizeof(char));
    _write(addr, "lolz", 4);

    assert(_read(addr, buff, 3) == SUCCESS);
    printf("%s\n", buff);

    free(buff);
    freeAll();

}

void read_incorrectAddr_wrongArgsReturned() {
    _init(12, 8);
    VA addr;
    _malloc(&addr, 20);

    char *buff = calloc(3 * sizeof(char), 3 * sizeof(char));
    _write(addr, "lolz", 4);
    assert(_read(addr, buff, -3) == WRONG_ARGUMENTS);
    freeAll();
}

void read_20_from_5_written_memLackReturned() {
    _init(1, 16);
    VA addr;
    _malloc(&addr, 5);

    char *buff = calloc(20 * sizeof(char), 20 * sizeof(char));
    _write(addr, "test1", 5);
    assert(_read(addr, buff, 20) == MEMORY_LACK);

    freeAll();
}


void run_read_tests() {

//    read_incorrectAddr_wrongArgsReturned();
//    read_20_from_5_written_memLackReturned();
    read_11_from_14_written_successReturned();
//    read_5_from_14_written_with_offset_successReturned();
//    read_3_with_4_written_successReturned();

}
