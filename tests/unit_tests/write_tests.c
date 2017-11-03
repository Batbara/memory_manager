#include <assert.h>
#include <stdlib.h>
#include "../../mmemory.h"
#include "../../paging.h"

void write_with_loading_successReturned() {
    _init(10, 4);
    VA addr;
    _malloc(&addr, 6);
    _malloc(&addr, 16);
    _malloc(&addr, 11);
    assert(_write(addr, "lolkek", 6) == SUCCESS);
    freeAll();
}

void write_neg_size_wrongArgsReturned() {
    _init(10, 4);
    VA addr;
    _malloc(&addr, 6);
    assert(_write(addr, "1t34f6", -6) == WRONG_ARGUMENTS);
    freeAll();
}

void write_too_long_data_unknownMistakeReturned() {
    _init(6, 4);
    VA addr;
    _malloc(&addr, 16);
    assert(_write(addr, "thisstringisjusttoolong", 23) == UNKNOWN_MISTAKE);

    freeAll();
}

void write_with_offset_successReturned() {
    _init(25, 4);
    VA addr;
    _malloc(&addr, 50);
    _malloc(&addr, 14);
    addr += 10;
    assert(_write(addr, "lol", 3) == SUCCESS);
    freeAll();
}

void write_in_not_malloc_unknownMistakeReturned() {
    _init(10, 4);
    VA addr;
    _malloc(&addr, 8);
    addr += 10;
    assert(_write(addr, "lol", 3) == UNKNOWN_MISTAKE);

    freeAll();
}

void write_twice_successReturned() {
    _init(10, 4);
    VA addr;
    _malloc(&addr, 8);
    _write(addr, "lol", 3);
    assert(_write(addr, "kek", 3) == SUCCESS);
    freeAll();

}

void run_write_tests() {

    write_neg_size_wrongArgsReturned();
    write_too_long_data_unknownMistakeReturned();
    write_with_loading_successReturned();
    write_with_offset_successReturned();
    write_in_not_malloc_unknownMistakeReturned();
    write_twice_successReturned();

}

