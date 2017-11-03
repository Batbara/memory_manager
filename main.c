#include <stdio.h>
#include "tests/unit_tests/init_tests.h"
#include "tests/unit_tests/malloc_tests.h"
#include "tests/unit_tests/free_tests.h"
#include "tests/unit_tests/read_tests.h"
#include "tests/unit_tests/write_tests.h"

int main() {

    run_init_tests();
    printf("passed init tests\n");
    run_malloc_tests();
    printf("passed malloc tests\n");
    run_free_tests();
    printf("passed free tests\n");
    run_write_tests();
    printf("passed write tests\n");

    printf("\nread results:\n");
    run_read_tests();
    printf("passed read tests\n");


}
