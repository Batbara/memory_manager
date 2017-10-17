#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../mmemory.h"
#include "../../paging.h"

void read_11bytes14bytesWrite_successReturned(){
    _init(10,4);
    VA addr = "0000000000000000";
    _malloc(&addr,20);
    char *buff = calloc(11*sizeof(char),11*sizeof(char));
    assert(_write(convertToVA(5),"teststring1234",14) == SUCCESS);
    assert(_read(convertToVA(7),buff,11)==SUCCESS);

}
void run_read_tests(){

}
