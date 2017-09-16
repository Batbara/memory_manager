#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include "mmemory.h"
#include "paging.h"

int main() {
    printf("Hello, World!\n");

    _init(4, 8);

    return 0;
}