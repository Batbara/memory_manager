#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../mmemory.h"
#include "../../paging.h"

void read_11bytes14bytesWrite_successReturned(){
    _init(10,4);
    VA addr;
    _malloc(&addr,10);
    _malloc(&addr,20);
    char *buff = calloc(11*sizeof(char),11*sizeof(char));
    assert(_write(addr,"teststring1234",14) == SUCCESS);
    addr+=2;
    assert(_read(addr,buff,11)==SUCCESS);
    printf("%s\n",buff);

}
void complex_test(){
    _init(15,4);
    VA addr;
    _malloc(&addr,60);
    _malloc(&addr,10);

    char *buff = calloc(11*sizeof(char),11*sizeof(char));
    assert(_write(addr,"teststring1234",14) == SUCCESS);
    addr+=5;
    assert(_read(addr,buff,3)==SUCCESS);
    printf("\n%s\n",buff); //tri

}
void read_5bytesFromDiskedPage_successReturned(){
    _init(20,4);
    VA addr;
    _malloc(&addr,80);
    char *buff = calloc(5*sizeof(char),5*sizeof(char));
    assert(_write(addr,"teststring1234",14) == SUCCESS);
    addr+=4;
    assert(_read(addr,buff,5)==SUCCESS);
    printf("%s\n",buff);

}

void read_3bytes4bytesWrite_successReturned(){
    _init(12,8);
    VA addr;
    _malloc(&addr,20);
    char *buff = calloc(3*sizeof(char),3*sizeof(char));
    assert(_write(addr,"lolz",4) == SUCCESS);
   assert(_read(addr,buff,3)==SUCCESS);
    printf("%s\n",buff);

}

void read_incorrectAddr_wrongArgsReturned(){
    _init(12,8);
    VA addr;
    _malloc(&addr,20);
    char *buff = calloc(3*sizeof(char),3*sizeof(char));
    assert(_write(addr,"lolz",4) == SUCCESS);
    assert(_read(addr,buff,-3)==WRONG_ARGUMENTS);
}

void read_20bytes5bytesWrite_memLackReturned(){
    _init(1,16);
    VA addr;
    _malloc(&addr,5);
    char *buff = calloc(20*sizeof(char),20*sizeof(char));
    assert(_write(addr,"test1",5) == SUCCESS);
    assert(_read(addr,buff,20)==MEMORY_LACK);

}


void run_read_tests(){
    read_11bytes14bytesWrite_successReturned();
    read_incorrectAddr_wrongArgsReturned();
   read_5bytesFromDiskedPage_successReturned();
    read_3bytes4bytesWrite_successReturned();
    read_20bytes5bytesWrite_memLackReturned();
    //complex_test();
}
