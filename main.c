#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <math.h>
#include "mmemory.h"
#include "paging.h"
#include "tests/unit_tests.h"
#include "tests/load_tests.h"

int main() {
     run_init_tests();
     run_malloc_tests();
     run_free_tests();
     run_write_tests();
     run_read_tests();
    return 0;
}
