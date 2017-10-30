#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <math.h>
#include "mmemory.h"
#include "paging.h"
#include "tests/unit_tests/init_tests.h"
#include "tests/unit_tests/malloc_tests.h"
#include "tests/unit_tests/free_tests.h"
#include "tests/unit_tests/read_tests.h"
#include "tests/unit_tests/write_tests.h"
#include "tests/load_tests.h"
unsigned createMask(unsigned a, unsigned b)
{
    unsigned r = 0;
    for (unsigned i=a; i<=b; i++)
        r |= 1 << i;

    return r;
}
int main() {
   //run_init_tests();
   //run_malloc_tests();
  //  run_free_tests();
  //  run_write_tests();
    run_read_tests();
  //  testPageNum();
//    testPageSize();
    //testPageSize();
    //testPageNum();


//
//
    /*int exponent = 10;
    unsigned mask = 0;
    for (unsigned i = 0; i < exponent; i++)
        mask |= 1 << i;
    unsigned  res = mask & 2617;
    printf("%d",res);
    return 0;*/
}
