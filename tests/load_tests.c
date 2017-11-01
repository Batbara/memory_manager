#include <stdio.h>
#include <time.h>
#include <stdlib.h>
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

        timer = clock();
        findBlockAddr((VA)addr);
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
    for (int pageSize = 32; pageSize < 32769; pageSize *= 2) {
        printf("size: %d, ",pageSize);
        fprintf(fp, "%d ", pageSize);
        time_t actual_time,actual_time1;
        double actual_time_sec;
        actual_time = clock();

        int i=_init(20, pageSize);
        printf("init: %d, ",i);
        VA addr;
        size_t sizeToAlloc = (size_t)15*pageSize;
        int m=_malloc(&addr,sizeToAlloc);
        printf("malloc: %d, ",m);
//        char * buffToWrite = calloc((size_t)2*pageSize, sizeof(char) * 2*pageSize);
//        for(int i = 0; i<pageSize; i++){
//            buffToWrite[i]='a';
//        }
        char * buffToWrite = "aa";
        addr+=5*pageSize-pageSize;
        int w=_write(addr, buffToWrite,(size_t )2);
        printf("write: %d\n",w);


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
    for (int pageNum = 1; pageNum < 1001; pageNum += 100) {
        printf("pageNum: %d, ",pageNum);
        fprintf(fp, "%d ", pageNum);
        time_t actual_time,actual_time1;
        double actual_time_sec;
        actual_time = clock();

        int i=_init(pageNum, 16);
        printf("init: %d, ",i);
        VA addr;
        size_t sizeToAlloc = (size_t)pageNum*16;
        int m=_malloc(&addr,sizeToAlloc);
        printf("malloc: %d, ",m);
        char * buffToWrite = calloc((size_t)2*pageNum, sizeof(char) * 2*pageNum);
        for(int i = 0; i<2*pageNum; i++){
            buffToWrite[i]='a';
        }
        addr+=5*pageNum-pageNum;
        int w=_write(addr, buffToWrite,(size_t )2*pageNum);
        printf("write: %d\n",w);


        actual_time1 = clock();
        actual_time_sec = difftime(actual_time1,actual_time);
        fprintf(fp,"%f\n",actual_time_sec);
    }

    fclose(fp);
}
