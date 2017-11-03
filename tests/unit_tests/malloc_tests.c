#include <assert.h>
#include "../../mmemory.h"
#include "../../paging.h"

void malloc_on_blocks_successReturned() {
    _init(30, 8);
    VA addr;
    assert(_malloc(&addr, 8) == SUCCESS);
    freeAll();
}

void malloc_negative_size_wrongArgsReturned() {
    _init(30, 8);
    VA addr;
    assert(_malloc(&addr, -4) == WRONG_ARGUMENTS);
}

void malloc_too_many_bytes_memLackReturned() {
    _init(5, 8);
    VA addr;
    assert(_malloc(&addr, 50) == MEMORY_LACK);
}

void malloc_bytes_successReturned() {
    _init(25, 8);
    VA addr;
    assert(_malloc(&addr, 5) == SUCCESS);
    freeAll();
}

void malloc_no_free_page_unknownMistakeReturned() {

    _init(20, 4);
    VA addr;
    _malloc(&addr, 80);
    assert(_malloc(&addr, 3) == UNKNOWN_MISTAKE);
    freeAll();

}


void run_malloc_tests() {

    malloc_negative_size_wrongArgsReturned();
    malloc_too_many_bytes_memLackReturned();
    malloc_no_free_page_unknownMistakeReturned();
    malloc_on_blocks_successReturned();
    malloc_bytes_successReturned();
}
