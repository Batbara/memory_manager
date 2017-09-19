#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include "mmemory.h"
#include "paging.h"
#include "tests/unit_tests.h"

int main() {
    //run_init_tests();
  /*  _init(10, 4);
    VA ptr = "00000000000000010";
    _malloc(&ptr, 5);
    int freeStatus = _free(ptr);
    if (freeStatus == SUCCESS) {
        printf("\nsuccess!");
    } else if (freeStatus == UNKNOWN_MISTAKE)
        printf("\nunknown mistake!");
    else
        printf("\nwrong args");*/
    //run_malloc_tests();
    run_free_tests();
    return 0;
}