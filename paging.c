#include "mmemory.h"
#include "paging.h"

#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <math.h>
#include "mmemory.h"
#include "paging.h"

void pushBlock(struct block **head, VA address, int blockSize) {
    struct block *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(struct block));
    current->address = address;
    current->data = NULL;
    current->isUsed = '0';
    current->blockSize = blockSize;
    current->next->next = NULL;
}

struct block *getBlockByAddress(struct block **head, VA address) {
    struct block *current = *head;
    while (current != NULL) {
        if (strcmp(current->address, address)==0)
            return current;
        current = current->next;
    }
    return NULL;
}

void setBlockUsage(struct block *memoryBlock, char usageFlag) {
    memoryBlock->isUsed = usageFlag;
    return;
}

VA convertToVA(int blockNumber) {
    VA address = calloc(ADDRESS_CAPACITY, sizeof(char) * ADDRESS_CAPACITY);

    for (int i = 0; i < ADDRESS_CAPACITY; i++) {
        address[i] = '0';
    }
    for (int bit = strlen(address); bit >= 0; bit--) {

        int remainder = blockNumber % 2;
        blockNumber /= 2;
        char bitToAdd = (char) (remainder + '0');
        address[bit] = bitToAdd;
    }
    return address;
}

struct page createPage(int pageOffset, struct block **memoryPool, int pageNum) {
    printf("\nPage offset is %d",pageOffset);
    struct page *newPage = (struct page*)malloc(sizeof(struct page));

    VA firstBlockAddress = convertToVA(pageOffset*pageNum);
    printf("\nFrist block address is: ");
    for (int bit = 0; bit<strlen(firstBlockAddress); bit++){
        printf("%c",firstBlockAddress[bit]);
    }
    struct block * firstBlock = getBlockByAddress(memoryPool,firstBlockAddress);

    newPage->firstBlock = firstBlock;
    newPage->pageNum = pageNum;

    return *newPage;
}
struct pageInfo createPageInfo(int pageOffset){
    struct pageInfo *info = (struct pageInfo*)malloc(sizeof(struct pageInfo));
    info->pageOffset = pageOffset;
    info->isUsed = '0';
    info->isAvailable='1';
    return *info;
}

int _init(int n, int szPage) {
    struct block *pool;
    struct page *virtualPages;
    struct pageInfo *table;

    int poolSize = n*szPage/BLOCK_SIZE;
    pool = (struct block*)malloc(poolSize*sizeof(struct block));
    pool->next = NULL;
    pool->data = NULL;
    pool->isUsed = '0';
    pool->address = convertToVA(0);
    printf("Initializing pool. Number of blocks: %d\n",poolSize/BLOCK_SIZE);
    printf("0 address: ");
    for (int bit = 0; bit<strlen(pool->address); bit++){
        printf("%c",pool->address[bit]);
    }
    printf("\n");
    for (int blockCount = 1; blockCount < poolSize/BLOCK_SIZE; blockCount++) {
        printf("%d address: ",blockCount);
        VA address = convertToVA(blockCount);
        for (int bit = 0; bit<strlen(address); bit++){
            printf("%c",address[bit]);
        }
        printf("\n");
        pushBlock(&pool, address, BLOCK_SIZE);
    }
    virtualPages = (struct page*)malloc(n * sizeof(struct page));
    table = (struct pageInfo*)malloc(n*sizeof(struct pageInfo));

    for (int pageNum = 0; pageNum < n; pageNum++) {
        int pageOffset = (int) (pageNum * log2((double)szPage));
        virtualPages[pageNum] = createPage(pageOffset,&pool,pageNum);
        table[pageNum] = createPageInfo(pageOffset);
    }

    return 0;
}

int isAddressValid(VA ptr) {
    if (strlen(ptr) != ADDRESS_CAPACITY)
        return WRONG_ARGUMENTS;
    for (int bit = 0; bit < ADDRESS_CAPACITY; bit++) {
        char value = ptr[bit];
        if (value != '0' && value != '1')
            return WRONG_ARGUMENTS;
    }
    return SUCCESS;
}

int _malloc(VA *ptr, size_t blockSize) {
    if (blockSize > MAX_POOL_SIZE || blockSize <= 0 || isAddressValid(*ptr) == WRONG_ARGUMENTS)
        return WRONG_ARGUMENTS;

    /*
     * алгоритм:
     * 1) вычисляем, сколько понадобится страниц выделить
     * 1) вычисляем номер страницы с таким адресом
     * 2)
     */
    return SUCCESS;
}