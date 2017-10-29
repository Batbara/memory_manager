#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../mmemory.h"
#include "../../paging.h"

void write_6bytes6bytesMalloc_successReturned() {
    _init(10, 4);
    VA addr = "0000000000000010";
    _malloc(&addr, 6);
    assert(_write(addr, "lolkek", 6) == SUCCESS);
    freeGlobalVars();
}

void write_6bytesToInvalidVA_wrongArgsReturned() {
    _init(10, 4);
    VA addr = "0000000000000010";
    _malloc(&addr, 6);
    addr = "0000000000050010";
    assert(_write(addr, "1t34f6", 6) == WRONG_ARGUMENTS);
    freeGlobalVars();
}

void write_23bytes16bytesMalloc_memLackReturned() {
    _init(6, 4);
    VA addr = "0000000000000010";
    _malloc(&addr, 16);
    assert(_write(addr, "thisstringisjusttoolong", 23) == MEMORY_LACK);

    freeGlobalVars();
}

void write_3bytes16Malloc_successReturned() {
    _init(25, 4);
    VA addr = "0000000001010100";
    _malloc(&addr, 16);
    assert(_write(convertToVA(90), "lol", 3) == SUCCESS);
    freeGlobalVars();
}
void write_in_not_malloced_unknownMistakeReturned() {
    _init(10, 4);
    VA addr = "0000000000001101";
    _malloc(&addr, 8);
    assert(_write(convertToVA(0), "lol", 3) == UNKNOWN_MISTAKE);

    freeGlobalVars();
}

void write_twice_successReturned() {
    _init(10, 4);
    VA addr = "0000000000000000";
    _malloc(&addr, 8);
    _write(convertToVA(0), "lol", 3);
    assert(_write(convertToVA(0), "kek", 3) == SUCCESS);

    freeGlobalVars();
}

void run_write_tests() {

    write_6bytesToInvalidVA_wrongArgsReturned();
    write_23bytes16bytesMalloc_memLackReturned();
    write_6bytes6bytesMalloc_successReturned();
    write_3bytes16Malloc_successReturned();
    write_in_not_malloced_unknownMistakeReturned();
    write_twice_successReturned();

}

