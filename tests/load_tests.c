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
    for (int pageSize = 2; pageSize < 32769; pageSize *= 2) {

        fprintf(fp, "%d ", pageSize);
        time_t actual_time,actual_time1;
        double actual_time_sec;
        actual_time = clock();

        _init(20, pageSize);
        int num = 15 * pageSize;
        VA addr = convertToVA(num);

        _write(addr, "test",4);


        actual_time1 = clock();
        actual_time_sec = difftime(actual_time1,actual_time);
        fprintf(fp,"%f\n",actual_time_sec);
    }



    fclose(fp);
}
void timer(){

}
void testPageNum() {
    FILE *fp;
    clock_t timer;
    fp = fopen("C:\\Users\\Barbarossa\\Documents\\memory_manager\\testPageNum.txt", "w+");

    fprintf(fp, "Testing malloc time from page size\n");
    for (int pageNum = 1; pageNum < 100001; pageNum += 10000) {
        fprintf(fp, "%d ", pageNum);
        time_t actual_time,actual_time1;
        double actual_time_sec;
        actual_time = clock();

        int i= _init(pageNum, 8);
        int num = 8*pageNum-4;
        VA addr = convertToVA(num);
        timer = clock();

       int m= _malloc(&addr,4);

        int c=_write(addr, "test",4);
        printf("%d",c);
        actual_time1 = clock();
        actual_time_sec = difftime(actual_time1,actual_time);
        fprintf(fp,"%f\n",actual_time_sec);
    }

    fclose(fp);
}
