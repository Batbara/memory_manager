#include <stdio.h>
#include <time.h>
#include "../paging.h"
#include "load_tests.h"

void testAddrCalculation() {
    FILE *fp;
    clock_t timer;
    fp = fopen("C:\\Users\\Barbarossa\\Documents\\memory_manager\\testAddrCalculation.txt", "w+");

    fprintf(fp, "Testing address calculation\n");
    _init(10, 128);
    for (int addr = 0; addr < 1000; addr += 100) {
        fprintf(fp, "%d ", addr);

        VA virtual = convertToVA(addr);
        timer = clock();
        findBlockAddr(virtual);
        timer = clock() - timer;
        float time = ((float) timer) / CLOCKS_PER_SEC * 10000;
        fprintf(fp, "%f\n", time);
    }

    fclose(fp);
}

void testPageSize() {
    FILE *fp;
    clock_t timer;
    fp = fopen("C:\\Users\\Barbarossa\\Documents\\memory_manager\\testPageSize.txt", "w+");

    fprintf(fp, "Testing malloc time from page size\n");
    for (int pageSize = 2; pageSize < 4096; pageSize *= 2) {
        fprintf(fp, "%d ", pageSize);
        _init(20, pageSize);
        int num = 15 * pageSize;
        VA addr = convertToVA(num);
        timer = clock();
        _malloc(&addr, 2);
        timer = clock() - timer;
        float time = ((float) timer) / CLOCKS_PER_SEC * 10000;
        fprintf(fp, "%f\n", time);
    }

    fclose(fp);
}

void testPageNum() {
    FILE *fp;
    clock_t timer;
    fp = fopen("C:\\Users\\Barbarossa\\Documents\\memory_manager\\testPageNum.txt", "w+");

    fprintf(fp, "Testing malloc time from page size\n");
    for (int pageNum = 1; pageNum < 1001; pageNum += 100) {
        fprintf(fp, "%d ", pageNum);
        _init(pageNum, 4);
        int num = (pageNum+4) / pageNum;
        VA addr = convertToVA(num);
        timer = clock();
        int status = _malloc(&addr, 2);
        printf("malloc status: %d\n", status);
        timer = clock() - timer;
        float time = ((float) timer) / CLOCKS_PER_SEC * 10000;
        fprintf(fp, "%f\n", time);
    }

    fclose(fp);
}
