#ifndef MEMORY_MANAGER_PAGING_H
#define MEMORY_MANAGER_PAGING_H


#define MAX_SIZE 32768
#define MAX_MEM_SIZE 64
#define MAX_PAGE_SIZE 16

#define SUCCESS 0
#define WRONG_ARGUMENTS -1
#define MEMORY_LACK -2
#define UNKNOWN_MISTAKE 1

#include "mmemory.h"
struct block {
    struct block *next;
    int address;
    char *data;
    char isUsed;
    int usedSize;
    int blockSize;
};
struct pageInfo {
    int physBlockAddr;
    int writeSize;
    char isAvailable; // 1 - страница в оперативной памяти; 0 - страница выгружена на диск
};
struct page {
    struct page *next;
    struct block *firstBlock;
    int freeSize;
    int pageNum;
    char isUsed;
    int offset;
};
struct diskCell {
    struct diskCell *next;
    struct page pageOnDisk;
    char *data;
    int usedSize;
    char isReserved;
    int pageNum;
};
struct userInput {
    int n;
    int szPage;
};
 struct block *memory;
 struct page *virtualPages;
 struct pageInfo *table;
 struct diskCell *memDisk;
 struct userInput *input;

void freeAll();

#endif

