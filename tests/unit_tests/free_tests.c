#include <assert.h>
#include "../../mmemory.h"
#include "../../paging.h"

void free_malloc_block_successReturned() {
    _init(10, 4);
    VA addr;
    _malloc(&addr, 16);

    assert(_free(addr) == SUCCESS);
    freeAll();
}

void free_invalidVA_wrongArgsReturned() {
    _init(10, 4);
    VA addr;
    _malloc(&addr, 16);

    addr += 100500;
    assert(_free(addr) == WRONG_ARGUMENTS);
    freeAll();
}

void free_from_start_of_block_successReturned() {
    _init(21, 8);
    VA addr;
    _malloc(&addr, 8);

    assert(_free(addr) == SUCCESS);
    freeAll();
}

void free_not_malloc_page_unknownMistakeReturned() {
    _init(15, 8);
    VA addr;
    _malloc(&addr, 10);
    addr += 22;
    assert(_free(addr) == UNKNOWN_MISTAKE);
    freeAll();
}

void free_with_offset_successReturned() {
    _init(22, 16);
    VA addr;
    _malloc(&addr, 10);
    addr+=2;

    assert(_free(addr) == SUCCESS);
    freeAll();
}

void run_free_tests() {
    free_malloc_block_successReturned();
    free_invalidVA_wrongArgsReturned();
    free_from_start_of_block_successReturned();
    free_not_malloc_page_unknownMistakeReturned();
    free_with_offset_successReturned();
}

