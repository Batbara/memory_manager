#include <stdio.h>
#include "../mmemory.h"
#include "../paging.h"
void _init_test(int testNum, int n, int szPage, int resultCode){
    printf("\tTest %d: ",testNum);
    int returnValue = _init(n,szPage);
    if(returnValue==resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}
void run_init_tests(){
    printf("Running _init tests:\n");
    _init_test(0,5,32,SUCCESS);
    _init_test(1,-2,64,WRONG_ARGUMENTS);
    _init_test(2,10,15,WRONG_ARGUMENTS);
    _init_test(3,20,128,SUCCESS);
    _init_test(4,100,1025,WRONG_ARGUMENTS);
}

