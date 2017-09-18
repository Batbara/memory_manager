#include <stdio.h>
#include "../mmemory.h"
#include "../paging.h"
#include <malloc.h>
#include <mem.h>
void _init_test(int testNum, int n, size_t szPage, int resultCode){
    printf("\tTest %d: ",testNum);
    int returnValue = _init(n,szPage);
    if(returnValue==resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}
void _malloc_test(int testNum, VA *ptr, size_t szBlock, int resultCode){
    printf("\tTest %d: ",testNum);
    int returnValue = _malloc(ptr,szBlock);
    if(returnValue==resultCode)
        printf("test passed!\n");
    else
        printf("test failed\n");
}
void freeGlobalVars(){
    free(pool);
    free(table);
    free(virtualPages);
}
void run_init_tests(){
    printf("Running _init tests:\n");
    _init_test(0,5,32,SUCCESS);
   // freeGlobalVars();

    _init_test(1,-2,64,WRONG_ARGUMENTS);
  //  freeGlobalVars();

    _init_test(2,10,15,WRONG_ARGUMENTS);
   // freeGlobalVars();

    _init_test(3,20,128,SUCCESS);
   // freeGlobalVars();

    _init_test(4,100,1025,WRONG_ARGUMENTS);
  //  freeGlobalVars();
}
void run_malloc_tests(){
    // начальные условия: 10 стр, размер 4
    _init(10,4);
    VA addr="00000000000000000";
    _malloc_test(0,&addr,16,SUCCESS);

    addr="0065465465";
    _malloc_test(1,&addr,4,WRONG_ARGUMENTS);

    addr="00000000000011110";
    _malloc_test(2,&addr,16,MEMORY_LACK);

}

