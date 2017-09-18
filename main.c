#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include "mmemory.h"
#include "paging.h"
#include "tests/unit_tests.h"

int main() {
    //run_init_tests();
//    _init(10,4);
//    VA ptr = "00000000000000010";
//    _malloc(&ptr,5);
    run_malloc_tests();
    return 0;
}