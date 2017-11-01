#include <assert.h>
#include "../../mmemory.h"
#include "../../paging.h"

void init_negpages_64size_wrongArgsReturned(){
    assert(_init(-2,64)==WRONG_ARGUMENTS);
}
void init_5pages16size_successReturned(){
    assert(_init(5,16)==SUCCESS);
};
void init_20pages4size_successReturned(){
assert(_init(20,4)==SUCCESS);
}
void init_11pages8size_successReturned(){
    assert(_init(11,8)==SUCCESS);
}
void init_25000pages80000size_memLackReturned(){
    assert(_init(25000,2048)==MEMORY_LACK);
}
void run_init_tests(){
    init_negpages_64size_wrongArgsReturned();
    init_5pages16size_successReturned();
    init_25000pages80000size_memLackReturned();
    init_20pages4size_successReturned();
    init_11pages8size_successReturned();

}

