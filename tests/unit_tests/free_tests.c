#include <assert.h>
#include "../../mmemory.h"
#include "../../paging.h"
void free_0VA_16malloc_successReturned(){
    _init(10, 4);
    VA addr = "0000000000000000";
    _malloc(&addr, 16);

    assert(_free(addr)==SUCCESS);
}
void free_invalidVA_16malloc_wrongArgsReturned(){
    _init(10, 4);
    VA addr = "0000000000000000";
    _malloc(&addr, 16);

    VA incorrectAddr = "00lol0kek00000";
    assert(_free(incorrectAddr)==WRONG_ARGUMENTS);
}
void free_150VA_32malloc_successReturned(){
    _init(21, 8);
    VA addr = "0000000010010110";
    _malloc(&addr, 32);

    assert(_free(addr)==SUCCESS);
}
void run_free_tests(){
    free_0VA_16malloc_successReturned();
    free_invalidVA_16malloc_wrongArgsReturned();
    free_150VA_32malloc_successReturned();
}

