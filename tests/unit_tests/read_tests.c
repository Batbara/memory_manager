#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../mmemory.h"
#include "../../paging.h"

void run_read_tests(){
    _init(10,4);
    VA addr = "0000000000000000";
    _malloc(&addr,20);
    char *buff = calloc(11*sizeof(char),11*sizeof(char));
    _write(convertToVA(5),"fuckthisshit!!",14);
    _read(convertToVA(7),buff,3);
    for(int i=0; i<3;i++){
        printf("%c",buff[i]);
    }
}
