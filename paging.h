#ifndef MEMORY_MANAGER_PAGING_H
#define MEMORY_MANAGER_PAGING_H


#define MAX_SIZE 32768
#define MAX_NUM_OF_PAGES_IN_RAM 10
#define MAX_NUM_OF_PAGES 1000
#define MAX_PAGE_SIZE 1024
#define ADDRESS_CAPACITY 16
#define BLOCK_SIZE 2

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
    char writeStatus;
    int usedSize;
    int blockSize;
};
struct pageInfo {
    int physBlockAddr;
    char isAvailable; // 1 - страница в оперативной памяти; 0 - страница выгружена на диск
};
struct page {
    struct page *next;
    struct block *firstBlock;
    int freeSize;
    int pageNum;
};
struct diskCell {
    struct diskCell *next;
    struct page pageOnDisk;
    char *data;
    int usedSize;
    int pageNum;
};
struct userInput {
    int n;
    int szPage;
    int system_constant;
};
 struct block *memory;
 struct page *virtualPages;
 struct pageInfo *table;
 struct diskCell *memDisk;
 struct userInput *input;

VA convertToVA(int num);
int findBlockAddr(VA ptr);
/*int convertToDecimal(VA addressToConvert);

int pushPage(struct page **head, struct page *pageToPush);
int allocateBlocks(struct block **firstBlock, VA *addressesToAlloc, int size);
struct page *createPage(int addrOffset, struct block **memoryPool);
struct pageInfo createPageInfo(int physBlockAddr, int addrOffset);
int isAddressValid(VA ptr);
VA *getPhysAddrPool(VA firstVA, VA lastVA);
int getPageNumberFromVA(VA address);
VA getVAoffset(VA address);
VA findBlockAddr(VA ptr);
VA *VApool(VA *ptr, size_t blockSize);
int isPageAvailable(int pageNum);
void saveInput(int n, int szPage);
int createBlocks();
int freeBlock(VA addr);
int writeToBlock(VA blockAddr, void *pBuffer);
int readFromPage(VA blockAddr, void *pBuffer, size_t size);
void initPhysMem();
int checkInitArguments(int n, int szPage);
struct diskCell *createDiskCell(struct page thePage, int addrOffset);
int pushDiskCell(struct diskCell **head, struct diskCell *cellToPush);
int loadPageToMem(int pageNum);
struct page *findPageInMem(int pageNum);*/
#endif //MEMORY_MANAGER_PAGING_H

