#include <assert.h>
#include "../../mmemory.h"
#include "../../paging.h"

void malloc_8bytes24VA_successReturned(){
    _init(30,8);
    VA addr = "0000000000011000";
    assert(_malloc(&addr,8)==SUCCESS);
}
void malloc_4bytesInvalidVA_wrongArgsReturned(){
    _init(30,8);
    VA addr = "0065465465";
    assert(_malloc(&addr,4)==WRONG_ARGUMENTS);
}
void malloc_50bytes38VA_memLackReturned(){
    _init(5,8);
    VA addr = "0000000000100110";
    assert(_malloc(&addr,50)==MEMORY_LACK);
}
void malloc_5bytes160VA_successReturned(){
    _init(25,8);
    VA addr = "0000000010100000";
    assert(_malloc(&addr,5)==SUCCESS);

}
void run_malloc_tests(){
    malloc_8bytes24VA_successReturned();
    malloc_4bytesInvalidVA_wrongArgsReturned();
    malloc_50bytes38VA_memLackReturned();
    malloc_5bytes160VA_successReturned();
}
