#ifndef MEMORY_MANAGER_PAGING_H
#define MEMORY_MANAGER_PAGING_H

#include "mmemory.h"
#define MAX_POOL_SIZE 32768
#define MAX_NUM_OF_PAGES 100
#define MAX_PAGE_SIZE 1024
#define ADDRESS_CAPACITY 16
#define BLOCK_SIZE 2

#define SUCCESS 0
#define WRONG_ARGUMENTS -1
#define MEMORY_LACK -2
#define UNKNOWN_MISTAKE 1
struct block {
    struct block *next;
    VA address;
    char *data;
    int blockSize;
    char isUsed;
};
struct pageInfo {
    int pageOffset;
    char isUsed;
    char isAvailable; // 1 - страница в оперативной памяти; 0 - страница выгружена на диск
};
struct page {
    struct block *firstBlock;
    int pageNum;
};
struct userInput {
    int n;
    int szPage;
};
struct userInput *input;
#endif //MEMORY_MANAGER_PAGING_H
