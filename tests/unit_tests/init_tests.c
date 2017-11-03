#include <assert.h>
#include "../../mmemory.h"
#include "../../paging.h"

void init_negpages_wrongArgsReturned() {

    assert(_init(-2, 64) == WRONG_ARGUMENTS);
}

void init_5_pages_16_size_successReturned() {
    assert(_init(5, 16) == SUCCESS);
    freeAll();
};

void init_20_pages_4_size_successReturned() {
    assert(_init(20, 4) == SUCCESS);
    freeAll();
}

void init_11_pages_8_size_successReturned() {
    assert(_init(11, 8) == SUCCESS);
    freeAll();
}

void init_25000_pages_80000_size_memLackReturned() {
    assert(_init(25000, 2048) == MEMORY_LACK);
}

void run_init_tests() {
    init_negpages_wrongArgsReturned();
    init_5_pages_16_size_successReturned();
    init_25000_pages_80000_size_memLackReturned();
    init_20_pages_4_size_successReturned();
    init_11_pages_8_size_successReturned();

}

