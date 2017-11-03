#include "mmemory.h"
#include "paging.h"

#include <malloc.h>
#include <mem.h>
#include <math.h>
#include <stdio.h>


void freeAll() {
    struct diskCell *dCell;

    while (memDisk != NULL) {
        dCell = memDisk;
        memDisk = memDisk->next;
        free(dCell);
    }

    struct block *b;
    while (memory != NULL) {
        b = memory;
        memory = memory->next;
        free(b);
    }

    struct page *p;
    while (virtualPages != NULL) {
        p = virtualPages;
        virtualPages = virtualPages->next;
        free(p);
    }
    if (input != NULL) {
        free(input);
    }
    if (table != NULL) {
        free(table);
    }
}

int pushBlock(struct block **head, int address, int blockSize) {
    struct block *current = *head;

    while (current->next != NULL) {
        current = current->next;
    }
    struct block *nextBlock = NULL;
    nextBlock = malloc(sizeof(struct block));

    if (nextBlock == NULL)
        return UNKNOWN_MISTAKE;

    nextBlock->address = address;
    nextBlock->blockSize = blockSize;
    nextBlock->usedSize = 0;
    nextBlock->data = NULL;
    nextBlock->isUsed = '0';
    nextBlock->next = NULL;

    current->next = nextBlock;
    return SUCCESS;
}

int pushPage(struct page **head, struct page *pageToPush) {
    struct page *newPage = (struct page *) malloc(sizeof(struct page));

    newPage->firstBlock = pageToPush->firstBlock;
    newPage->next = NULL;
    newPage->offset = pageToPush->offset;
    newPage->pageNum = pageToPush->pageNum;
    newPage->freeSize = pageToPush->freeSize;
    struct page *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newPage;
    return SUCCESS;
}

int pushDiskCell(struct diskCell **head, struct diskCell *cellToPush) {
    if (cellToPush == NULL)
        return UNKNOWN_MISTAKE;
    struct diskCell *current = *head;
    if ((*head)->pageNum == -1) {
        (*head)->pageOnDisk = cellToPush->pageOnDisk;
        (*head)->pageNum = cellToPush->pageNum;
        (*head)->next = NULL;
        (*head)->isReserved = cellToPush->isReserved;
        return SUCCESS;
    }
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = cellToPush;
    return SUCCESS;
}

struct diskCell *findDiskCell(struct diskCell **head, int pageOffset) {
    struct diskCell *current = *head;
    while (current->next != NULL) {
        if (current->pageNum == pageOffset)
            return current;
        current = current->next;
    }
    return current;
}

int allocateBlock(struct block *firstBlock, int usedSize) {

    if (firstBlock->isUsed == '1' && firstBlock->usedSize == firstBlock->blockSize)
        return UNKNOWN_MISTAKE;
    firstBlock->data = calloc((size_t) input->szPage, sizeof(char) * input->szPage);
    firstBlock->isUsed = '1';
    firstBlock->usedSize += usedSize;
    return SUCCESS;

}

struct block *findBlockNode(int blockAddr) {
    struct block *current = memory;
    while (current->next != NULL) {
        if (blockAddr == current->address) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

struct page *createPage(int pageNum) {

    struct page *newPage = (struct page *) malloc(sizeof(struct page));

    newPage->next = NULL;
    newPage->pageNum = pageNum;
    newPage->freeSize = input->szPage;
    newPage->firstBlock = NULL;
    newPage->isUsed = '0';
    newPage->offset = 0;

    return newPage;
}

struct page *getPage(int pageNum) {
    struct page *current = virtualPages;
    if (current->pageNum == pageNum && current->next == NULL)
        return current;
    while (current->next != NULL) {
        if (current->pageNum == pageNum)
            return current;
        current = current->next;
    };
    if (current->pageNum == pageNum)
        return current;
    return NULL;
}

struct block *findFreeBlock() {
    struct block *current = memory;
    while (current->next != NULL) {
        if (current->isUsed == '0') {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

struct diskCell *createDiskCell(struct page thePage, int pageNum) {
    struct diskCell *cell = (struct diskCell *) malloc(sizeof(struct diskCell));
    cell->pageOnDisk = thePage;
    cell->usedSize = 0;
    cell->data = NULL;
    cell->pageNum = pageNum;
    cell->next = NULL;
    cell->isReserved = '0';
    return cell;
}

struct pageInfo createPageInfo() {
    struct pageInfo *info = (struct pageInfo *) malloc(sizeof(struct pageInfo));

    info->isAvailable = '0';
    info->physBlockAddr = -1;
    info->writeSize = 0;
    return *info;
}

int isAddressValid(VA ptr) {
    if ((int) ptr > input->n * input->szPage) {
        return WRONG_ARGUMENTS;
    }
    return SUCCESS;
}

int isSzPageValid(int szPage) {
    if (szPage <= 0)
        return WRONG_ARGUMENTS;
    double exponent = log2((double) szPage);
    if (exponent - ceil(exponent) == 0)
        return SUCCESS;
    else return WRONG_ARGUMENTS;
}

int getPageNumberFromVA(VA ptr) {
    int szPage = input->szPage;
    int exponent = (int) (log2((double) szPage));
    int pageNum = ((int) ptr) >> exponent;
    return pageNum;
}

VA getVAoffset(VA ptr) {
    int szPage = input->szPage;
    int exponent = (int) (log2((double) szPage));
    int decimalPtr = (int) ptr;

    unsigned mask = 0;
    for (unsigned i = 0; i < exponent; i++)
        mask |= 1 << i;
    VA offset = (VA) (mask & decimalPtr);
    return offset;
}

int isPageAllocated(int pageNum) {
    struct page *currPage = getPage(pageNum);

    if (currPage->isUsed == '1')
        return SUCCESS;

    struct diskCell *cell = findDiskCell(&memDisk, pageNum);
    if (cell->isReserved != '1') {
        return UNKNOWN_MISTAKE;
    } else return SUCCESS;

}

int isPageInMem(int pageNum) {
    struct page *current = virtualPages->next;
    if (current->pageNum == pageNum && current->next == NULL)
        return SUCCESS;
    while (current->next != NULL) {
        if (table[current->pageNum].isAvailable == '1') {

            if (current->pageNum == pageNum)
                return SUCCESS;
        }

        current = current->next;
    };
    if (current->pageNum == pageNum && table[current->pageNum].isAvailable == '1')
        return SUCCESS;
    return UNKNOWN_MISTAKE;
}

struct page *findPageInMem(int pageNum);


struct page *findOptimalPage(int anchorNum) {
    struct page *current = virtualPages->next;
    while (current->next != NULL) {
        if (table[current->pageNum].isAvailable == '1') {
            if (current->pageNum == anchorNum) {
                continue;
            } else
                return current;
        }
        current = current->next;
    };
    return NULL;

}

void freeBlock(struct block *physBlock) {

    physBlock->isUsed = '0';
    physBlock->usedSize = 0;
    if (physBlock->data != "")
        free(physBlock->data);
}

void copyDataToDisk(struct page *pageToUnload) {
    struct block *firstBlock = pageToUnload->firstBlock;
    struct diskCell *cell = findDiskCell(&memDisk, pageToUnload->pageNum);
    cell->usedSize = firstBlock->usedSize;
    cell->isReserved = '1';
    int writeSize = table[pageToUnload->pageNum].writeSize;

    if (writeSize != 0) {
        cell->data = calloc((size_t) writeSize, sizeof(char) * writeSize);
        memcpy(cell->data, firstBlock->data, (size_t) writeSize);
    }
    freeBlock(firstBlock);
}

struct block *unloadBlockInMem(int anchorPage) {
    struct page *pageToUnload = findOptimalPage(anchorPage);
    if (pageToUnload == NULL)
        return NULL;
    copyDataToDisk(pageToUnload);

    table[pageToUnload->pageNum].isAvailable = '0';
    table[pageToUnload->pageNum].writeSize = 0;
    table[pageToUnload->pageNum].physBlockAddr = -1;

    struct block *firstBlock = pageToUnload->firstBlock;
    pageToUnload->firstBlock = NULL;
    pageToUnload->isUsed = '0';
    return firstBlock;
}

void copyPageDataInMemory(struct page *pageToLoad) {

    int pageNum = pageToLoad->pageNum;
    struct block *firstBlock = pageToLoad->firstBlock;
    struct diskCell *cell = findDiskCell(&memDisk, pageNum);
    firstBlock->usedSize = cell->usedSize;
    firstBlock->isUsed = '1';
    if (firstBlock->usedSize != 0) {
        memcpy(firstBlock->data, cell->data, (size_t) firstBlock->usedSize);
    } else {
        firstBlock->data = calloc((size_t) input->szPage, sizeof(char) * input->szPage);
    }
}

struct page *findPageInMem(int pageNum) {
    struct page *current = virtualPages->next;
    if (current->pageNum == pageNum && current->next == NULL)
        return current;
    while (current->next != NULL) {
        if (table[current->pageNum].isAvailable == '1') {

            if (current->pageNum == pageNum)
                return current;
        }

        current = current->next;
    };
    if (current->pageNum == pageNum)
        return current;
    return NULL;
}

struct page *findFreePage(int usingSize) {
    struct page *current = virtualPages->next;

    while (current->next != NULL) {
        if (current->freeSize >= usingSize || current->freeSize == input->szPage) {
            return current;
        }
        current = current->next;
    };
    if (current->freeSize >= usingSize || current->freeSize == input->szPage) {
        return current;
    }
    return NULL;
}

int loadPageToMem(int pageNum) {
    struct page *pageToLoad = getPage(pageNum);
    struct block *freeBlock = findFreeBlock();
    if (freeBlock == NULL) {
        freeBlock = unloadBlockInMem(pageNum);
        if (freeBlock == NULL) {
            return UNKNOWN_MISTAKE;
        }

    }
    pageToLoad->firstBlock = freeBlock;
    copyPageDataInMemory(pageToLoad);
    table[pageNum].isAvailable = '1';
    pageToLoad->isUsed = '1';
    return SUCCESS;

}

int freeBlocks(struct page *firstPage) {
    struct page *current = firstPage;

    if (current->firstBlock->isUsed != '1')
        return UNKNOWN_MISTAKE;
    struct block *physBlock = current->firstBlock;

    freeBlock(physBlock);
    return SUCCESS;
}

int writeInPage(struct page *currPage, char inputData, int position) {
    struct block *blockToWrite = currPage->firstBlock;
    if (blockToWrite == NULL || blockToWrite->isUsed == '0' || blockToWrite->data == NULL)
        return UNKNOWN_MISTAKE;
    blockToWrite->data[position] = inputData;
    return SUCCESS;
}

int readFromPage(struct page *currPage, void *pBuffer, int offset) {
    struct block *blockToRead = currPage->firstBlock;
    if (blockToRead == NULL || blockToRead->isUsed == '0' || blockToRead->data == "")
        return UNKNOWN_MISTAKE;
    char dataValue = blockToRead->data[offset];
    memcpy(pBuffer, &dataValue, 1);
    return SUCCESS;
}

void initPhysMem() {
    int memSize = (MAX_MEM_SIZE) / input->szPage;
    memory = (struct block *) malloc(memSize * sizeof(struct block));

    memory->next = NULL;
    memory->blockSize = input->szPage;
    memory->data = NULL;
    memory->isUsed = '0';
    memory->address = 0;
    memory->usedSize = 0;
}

int checkInitArguments(int n, int szPage) {
    if (n < 0 || isSzPageValid(szPage) != SUCCESS)
        return WRONG_ARGUMENTS;
    int maxPageNum = MAX_SIZE / szPage;
    if (n > maxPageNum || szPage > MAX_PAGE_SIZE || n * szPage > MAX_SIZE) {
        return MEMORY_LACK;
    }
    return SUCCESS;
}

void saveInput(int n, int szPage) {
    input = malloc(sizeof(struct userInput));
    input->n = n;
    input->szPage = szPage;
}

int createBlocks() {
    int poolSize = MAX_MEM_SIZE / input->szPage;

    for (int blockCount = 1; blockCount < poolSize; blockCount++) {


        int status = pushBlock(&memory, blockCount, input->szPage);
        if (status != SUCCESS)
            return UNKNOWN_MISTAKE;
    }
    return SUCCESS;
}

int mapPageWithBlock(struct page *newPage, int sizeToAlloc) {
    int physBlockAddr = findFreeBlock()->address;
    int pageNum = newPage->pageNum;
    table[pageNum].physBlockAddr = physBlockAddr;
    table[pageNum].isAvailable = '1';

    struct block *firstBlock = findBlockNode(physBlockAddr);
    int status = allocateBlock(firstBlock, sizeToAlloc);
    if (status == SUCCESS) {

        newPage->isUsed = '1';
        newPage->firstBlock = firstBlock;
        newPage->freeSize -= sizeToAlloc;
    }
    return status;
}

int _init(int n, int szPage) {
    int checkInitArgumentsStatus = checkInitArguments(n, szPage);
    if (checkInitArgumentsStatus != SUCCESS)
        return checkInitArgumentsStatus;

    saveInput(n, szPage);
    initPhysMem();

    int createBlocksStatus = createBlocks();
    if (createBlocksStatus != SUCCESS)
        return createBlocksStatus;

    virtualPages = (struct page *) malloc(n * sizeof(struct page));
    virtualPages->next = NULL;
    virtualPages->firstBlock = findBlockNode(0);

    table = (struct pageInfo *) malloc(n * sizeof(struct pageInfo));
    memDisk = (struct diskCell *) malloc((MAX_SIZE / szPage) * sizeof(struct diskCell));
    memDisk->next = NULL;
    memDisk->pageNum = -1;

    if (virtualPages == NULL || table == NULL)
        return UNKNOWN_MISTAKE;

    for (int pageNum = 0; pageNum < n; pageNum++) {

        struct page *thePage = createPage(pageNum);

        struct diskCell *cell = createDiskCell(*thePage, pageNum);
        int pushingStatus = pushDiskCell(&memDisk, cell);

        if (pushingStatus != SUCCESS)
            return UNKNOWN_MISTAKE;

        table[pageNum] = createPageInfo();

        pushingStatus = pushPage(&virtualPages, thePage);
        if (pushingStatus != SUCCESS)
            return UNKNOWN_MISTAKE;

    }

    return SUCCESS;
}

int _malloc(VA *ptr, size_t mallocSize) {
    int maxMemorySize = input->n * input->szPage;
    if ((int) mallocSize <= 0) {
        return WRONG_ARGUMENTS;
    }
    if (mallocSize > maxMemorySize)
        return MEMORY_LACK;
    struct page *freePage = findFreePage(mallocSize);
    if (freePage == NULL) {
        return UNKNOWN_MISTAKE;
    }
    int firstPageNum = freePage->pageNum;
    int virtAddr = firstPageNum * input->szPage + freePage->offset;
    *ptr = (VA) virtAddr;

    int size = mallocSize;
    int pagesToAlloc = (int) ceil((double) mallocSize / input->szPage);
    for (int pageNum = firstPageNum; pageNum < firstPageNum + pagesToAlloc; pageNum++) {
        int allocatedStaus = isPageAllocated(pageNum);
        if (allocatedStaus == SUCCESS) {
            return UNKNOWN_MISTAKE;
        }
        int freeSizeInPage = getPage(pageNum)->freeSize;
        int sizeToAlloc = 0;
        if (size < freeSizeInPage) {
            sizeToAlloc = size;
        } else {
            size -= getPage(pageNum)->freeSize;
            sizeToAlloc = freeSizeInPage;
        }
        struct page *newPage = getPage(pageNum);

        struct block *freeBlock = findFreeBlock();
        if (freeBlock != NULL) {
            int status = mapPageWithBlock(newPage, sizeToAlloc);
            if (status != SUCCESS)
                return status;
        } else {
            struct diskCell *cell = findDiskCell(&memDisk, pageNum);
            cell->isReserved = '1';
            newPage->isUsed = '1';
        }
    }
    return SUCCESS;

}

int _free(VA ptr) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS)
        return WRONG_ARGUMENTS;

    int pageNum = getPageNumberFromVA(ptr);
    if (isPageAllocated(pageNum) != SUCCESS) {
        return UNKNOWN_MISTAKE;
    }
    if (isPageInMem(pageNum) == SUCCESS) {

        struct page *currPage = findPageInMem(pageNum);
        int freeStatus = freeBlocks(currPage);

        return freeStatus;
    }
    return SUCCESS;
}

int _write(VA ptr, void *pBuffer, size_t szBuffer) {
    if ((int) szBuffer <= 0)
        return WRONG_ARGUMENTS;

    if (szBuffer > input->n * input->szPage) {
        return MEMORY_LACK;
    }

    char *data = (char *) pBuffer;

    VA address = ptr;
    for (int i = 0; i < szBuffer; i++) {
        address += 1;
        int currPageNum = getPageNumberFromVA(address);
        int allocedStatus = isPageAllocated(currPageNum);
        if (allocedStatus != SUCCESS) {
            return allocedStatus;
        }

        if (isPageInMem(currPageNum) != SUCCESS) {
            loadPageToMem(currPageNum);
        }

        struct page *currPage = getPage(currPageNum);
        if (currPage == NULL) {
            return UNKNOWN_MISTAKE;
        }

        int offset = (int) getVAoffset(address);
        char dataChar = data[i];
        int writeStatus = writeInPage(currPage, dataChar, offset);
        if (writeStatus != SUCCESS)
            return writeStatus;
        table[currPageNum].writeSize += 1;
    }

    return SUCCESS;
}

int _read(VA ptr, void *pBuffer, size_t szBuffer) {
    if ((int) szBuffer <= 0)
        return WRONG_ARGUMENTS;

    int n = input->n;
    int szPage = input->szPage;
    if (szBuffer > n * szPage) {
        return MEMORY_LACK;
    }

    VA address = ptr;
    for (int i = 0; i < szBuffer; i++) {
        address += 1;
        int currPageNum = getPageNumberFromVA(address);
        if (isPageInMem(currPageNum) != SUCCESS) {
            loadPageToMem(currPageNum);
        }
        struct page *currPage = getPage(currPageNum);
        if (currPage == NULL) {
            return UNKNOWN_MISTAKE;
        }

        int offset = (int) getVAoffset(address);
        char data = '0';
        int readStatus = readFromPage(currPage, &data, offset);
        if (readStatus != SUCCESS)
            return readStatus;
        strcat(pBuffer, &data);
    }

    return SUCCESS;
}