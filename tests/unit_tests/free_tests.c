#include <assert.h>
#include "../../mmemory.h"
#include "../../paging.h"
void free_0VA_16malloc_successReturned(){
    _init(10, 4);
    VA addr;
    _malloc(&addr, 16);

    assert(_free(addr)==SUCCESS);
}
void free_invalidVA_16malloc_wrongArgsReturned(){
    _init(10, 4);
    VA addr;
    _malloc(&addr, 16);

    addr+=100500;
    assert(_free(addr)==WRONG_ARGUMENTS);
}
void free_150VA_32malloc_successReturned(){
    _init(21, 8);
    VA addr;
    _malloc(&addr, 8);

    assert(_free(addr)==SUCCESS);
}
void free_20VA_10malloc_unknownMistakeReturned(){
    _init(15, 8);
    VA addr;
    _malloc(&addr, 10);
    addr+=22;
    assert(_free(addr)==UNKNOWN_MISTAKE);
}
void free_272VA_10malloc_successReturned(){
    _init(22, 16);
    VA addr;
    _malloc(&addr, 10);

    assert(_free(addr)==SUCCESS);
}
void run_free_tests(){
    free_0VA_16malloc_successReturned();
    free_invalidVA_16malloc_wrongArgsReturned();
    free_150VA_32malloc_successReturned();
    free_20VA_10malloc_unknownMistakeReturned();
    free_272VA_10malloc_successReturned();
}

