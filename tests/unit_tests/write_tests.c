#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../mmemory.h"
#include "../../paging.h"

void write_6bytes6bytesMalloc_successReturned(){
    _init(10,4);
    VA addr = "0000000000000010";
    _malloc(&addr,6);
    assert(_write(addr,"lolkek",6)==SUCCESS);
}
void write_6bytesToInvalidVA_wrongArgsReturned(){
    _init(10,4);
    VA addr = "0000000000000010";
    _malloc(&addr,6);
    addr = "0000000000050010";
    assert(_write(addr,"1t34f6",6)==WRONG_ARGUMENTS);
}
void write_23bytes16bytesMalloc_memLackReturned(){
    _init(6,4);
    VA addr = "0000000000000010";
    _malloc(&addr,16);
    assert(_write(addr,"thisstringisjusttoolong",23)==MEMORY_LACK);
}

void write_3bytesbytesMalloc_memLackReturned(){
    _init(25,4);
    VA addr = "0000000001010100";
    _malloc(&addr,16);
    assert(_write(convertToVA(90),"lol",3)==SUCCESS);
}
void run_write_tests(){
    write_6bytesToInvalidVA_wrongArgsReturned();
    write_23bytes16bytesMalloc_memLackReturned();
    write_6bytes6bytesMalloc_successReturned();

    write_3bytesbytesMalloc_memLackReturned();

}

