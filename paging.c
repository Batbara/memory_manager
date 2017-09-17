#include "mmemory.h"
#include "paging.h"

#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <math.h>
#include "mmemory.h"
#include "paging.h"


int pushBlock(struct block **head, VA address, int blockSize) {
    struct block *current = *head;

    while (current->next != NULL) {
        current = current->next;
    }
    struct block *nextBlock = NULL;
    nextBlock = malloc(sizeof(struct block));

    if (nextBlock == NULL)
        return UNKNOWN_MISTAKE;

    nextBlock->address = address;
    nextBlock->data = NULL;
    nextBlock->isUsed = '0';
    nextBlock->blockSize = blockSize;
    nextBlock->next = NULL;

    current->next = nextBlock;
    return SUCCESS;
}

struct block *getBlockByAddress(struct block **head, VA address) {
    struct block *current = *head;
    while (current != NULL) {
        char *blockAdress = current->address;
        if (strcmp(blockAdress, address) == 0)
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

void initializePool(struct block **pool) {
    (*pool)->next = NULL;
    (*pool)->data = NULL;
    (*pool)->isUsed = '0';
    (*pool)->address = convertToVA(0);
}

struct page createPage(int pageOffset, struct block **memoryPool, int pageNum) {
   // printf("\nPage offset is %d", pageOffset);
    struct page *newPage = (struct page *) malloc(sizeof(struct page));

    VA firstBlockAddress = convertToVA(pageOffset);
//    printf("\nFrist block address is: ");
//    for (int bit = 0; bit < strlen(firstBlockAddress); bit++) {
//        printf("%c", firstBlockAddress[bit]);
//    }
    struct block *firstBlock = getBlockByAddress(memoryPool, firstBlockAddress);

    newPage->firstBlock = firstBlock;
    newPage->pageNum = pageNum;

    return *newPage;
}

struct pageInfo createPageInfo(int pageOffset) {
    struct pageInfo *info = (struct pageInfo *) malloc(sizeof(struct pageInfo));
    info->pageOffset = pageOffset;
    info->isUsed = '0';
    info->isAvailable = '1';
    return *info;
}


int isAddressValid(VA ptr) {
    if (strlen(ptr) != ADDRESS_CAPACITY + 1)
        return WRONG_ARGUMENTS;
    for (int bit = 0; bit < ADDRESS_CAPACITY; bit++) {
        char value = ptr[bit];
        if (value != '0' && value != '1')
            return WRONG_ARGUMENTS;
    }
    return SUCCESS;
}

int isSzPageValid(int szPage) {
    if (szPage <= 0 || szPage>MAX_PAGE_SIZE)
        return WRONG_ARGUMENTS;
    double exponent = log2((double) szPage);
    if (exponent - ceil(exponent) == 0)
        return SUCCESS;
    else return WRONG_ARGUMENTS;
}

int _init(int n, int szPage) {
    if (n < 1 || n > MAX_NUM_OF_PAGES || isSzPageValid(szPage) != SUCCESS)
        return WRONG_ARGUMENTS;
    input = malloc(sizeof(struct userInput));
    input->n = n;
    input->szPage = szPage;

    struct block *pool = NULL;
    struct page *virtualPages = NULL;
    struct pageInfo *table = NULL;

    int poolSize = n * szPage / BLOCK_SIZE;
    pool = (struct block *) malloc(poolSize * sizeof(struct block));
    if (pool == NULL)
        return UNKNOWN_MISTAKE;
    pool->next = NULL;
    pool->data = NULL;
    pool->isUsed = '0';
    pool->blockSize = BLOCK_SIZE;
    pool->address = convertToVA(0);
    /*printf("Initializing pool. Number of blocks: %d\n", poolSize);
    printf("0 address: ");
    for (int bit = 0; bit < strlen(pool->address); bit++) {
        printf("%c", pool->address[bit]);
    }
    printf("\n");*/
    for (int blockCount = 1; blockCount < poolSize; blockCount++) {
        // printf("%d address: ", blockCount);
        VA address = convertToVA(blockCount);
        /*for (int bit = 0; bit < strlen(address); bit++) {
            printf("%c", address[bit]);
        }
        printf("\n");*/

       int status= pushBlock(&pool, address, BLOCK_SIZE);
        if(status!=SUCCESS)
            return UNKNOWN_MISTAKE;
    }
    virtualPages = (struct page *) malloc(n * sizeof(struct page));
    table = (struct pageInfo *) malloc(n * sizeof(struct pageInfo));

    if (virtualPages==NULL || table==NULL)
        return UNKNOWN_MISTAKE;

    for (int pageNum = 0; pageNum < n; pageNum++) {
        int pageOffset = (int) (pageNum * log2((double) szPage));
        virtualPages[pageNum] = createPage(pageOffset, &pool, pageNum);
        table[pageNum] = createPageInfo(pageOffset);
    }

    return SUCCESS;
}


int _malloc(VA *ptr, size_t blockSize) {
    int maxMemorySize = input->n * input->szPage;
    if (blockSize > maxMemorySize || blockSize <= 0 || isAddressValid(*ptr) == WRONG_ARGUMENTS) {
        printf("Wrong arguments!\n");
        return WRONG_ARGUMENTS;
    }

    /*
     * алгоритм:
     * 1) вычисляем номер страницы с таким адресом
     * 2)
     */
    return SUCCESS;
}