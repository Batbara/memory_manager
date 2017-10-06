#include "mmemory.h"
#include "paging.h"

#include <malloc.h>
#include <mem.h>
#include <math.h>
#include <stdio.h>

/*
 * Добавление блока в связный список
 */
int pushBlock(struct block **head, int address) {
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
    nextBlock->next = NULL;

    current->next = nextBlock;
    return SUCCESS;
}

/*
 * Преобразование десятичного номера блока в двоичный адрес
 */
VA convertToVA(int num) {
    unsigned int mask = MAX_SIZE;   //mask = [1000 0000 0000 0000]
    VA address = calloc(ADDRESS_CAPACITY, sizeof(char) * ADDRESS_CAPACITY);
    while (mask > 0) {

        ((num & mask) == 0) ? strncat(address, "0", 1) : strncat(address, "1", 1);

        mask = mask >> 1;  // Right Shift
    }
    return address;
}

int pushPage(struct page **head, struct page *pageToPush) {

    struct page *newPage = (struct page *) malloc(sizeof(struct page));


    newPage->firstBlock = pageToPush->firstBlock;
    newPage->next = NULL;
    newPage->pageNum = pageToPush->pageNum;
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
    if ((*head)->pageOffset == -1) {
        (*head)->pageOnDisk = cellToPush->pageOnDisk;
        (*head)->pageOffset = cellToPush->pageOffset;
        (*head)->next = NULL;
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
        if (current->pageOffset == pageOffset)
            return current;
        current = current->next;
    }
    return current;
}

void deleteDiskCell(struct diskCell **head, int pageOffset) {
    struct diskCell *current = *head;
    struct diskCell *prev;

    if (current != NULL && current->pageOffset == pageOffset) {
        *head = current->next;
        free(current);
        return;
    }

    while (current != NULL && current->pageOffset != pageOffset) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) return;

    prev->next = current->next;

    free(current);
}

void deletePage(struct page **head, struct page *pageToDel) {
    struct page *current = *head;
    struct page *prev;

    if (current != NULL && current == pageToDel) {
        *head = current->next;
        free(current);
        return;
    }

    while (current != NULL && current != pageToDel) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) return;

    prev->next = current->next;

    free(current);
}

int allocateBlock(struct block **firstBlock, int addrToAlloc) {
    struct block *current = *firstBlock;

    while (current->next != NULL) {
        int blockAddress = current->address;
        if (blockAddress == addrToAlloc) {
            if (current->isUsed == '1')
                return UNKNOWN_MISTAKE;
            current->isUsed = '1';
            return SUCCESS;
        }
        current = current->next;
    }
    if (current->address == addrToAlloc) {
        if (current->isUsed == '1')
            return UNKNOWN_MISTAKE;
        current->isUsed = '1';
        return SUCCESS;
    }
}

/*
 * Преборазование виртуального адреса в десятичнй номер
 */
int convertToDecimal(VA addressToConvert) {
    int decimal = 0;
    for (int bit = strlen(addressToConvert) - 1, exp = 0; bit >= 0; bit--, exp++) {
        int number = addressToConvert[bit] - '0';
        decimal += number << exp;
    }
    return decimal;
}

struct block *findBlockNode(int blockAddr) {
    struct block *current = pool;
    while (current->next != NULL) {
        if (blockAddr == current->address) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*
 * Создание виртуальной страницы
 */
struct page *createPage(int pageOffset, int pageNum) {

    struct page *newPage = (struct page *) malloc(sizeof(struct page));

    int firstBlockAddress = pageOffset;
    struct block *firstBlock = findBlockNode(firstBlockAddress);

    newPage->firstBlock = firstBlock;
    newPage->next = NULL;
    newPage->pageNum = pageNum;

    return newPage;
}

struct diskCell *createDiskCell(struct page thePage, int pageOffset) {
    struct diskCell *cell = (struct diskCell *) malloc(sizeof(struct diskCell));
    cell->pageOnDisk = thePage;
    cell->pageOffset = pageOffset;
    cell->next = NULL;
    return cell;
}

/*
 * Создание "строки" в таблице страниц
 */
struct pageInfo createPageInfo(int firstBlockOffset, int pageOffset) {
    struct pageInfo *info = (struct pageInfo *) malloc(sizeof(struct pageInfo));
    info->firstBlockOffset = firstBlockOffset;

    if (pageOffset > MAX_NUM_OF_PAGES_IN_RAM) {
        info->isAvailable = '0';
    } else
        info->isAvailable = '1';
    return *info;
}

/*
 * Проверка корректности виртуального адреса
 */
int isAddressValid(VA ptr) {
    if (strlen(ptr) != ADDRESS_CAPACITY)
        return WRONG_ARGUMENTS;
    for (int bit = 0; bit < ADDRESS_CAPACITY; bit++) {
        char value = ptr[bit];
        if (value != '0' && value != '1')
            return WRONG_ARGUMENTS;
    }
    if (convertToDecimal(ptr) > input->n * input->szPage) {
        return WRONG_ARGUMENTS;
    }
    return SUCCESS;
}

/*
 * Проверка рамера страницы
 */
int isSzPageValid(int szPage) {
    if (szPage <= 0 || szPage > MAX_PAGE_SIZE)
        return WRONG_ARGUMENTS;
    double exponent = log2((double) szPage);
    if (exponent - ceil(exponent) == 0)
        return SUCCESS;
    else return WRONG_ARGUMENTS;
}

/*
 * Получение пула адресов блоков, которые
 * находятся в интервале [firstVA, lastVA] виртуальных адресов
 */
int *getPhysAddrPool(int firstAddr, int lastAddr) {
    int size = lastAddr - firstAddr;
    int *addrPool = (int *) malloc(size * sizeof(int));
    for (int addr = firstAddr, count = 0; addr < lastAddr; addr++, count++) {
        addrPool[count] = addr;
    }
    return addrPool;
}

/*
 * Получение номера виртуальной страницы из виртуального адреса
 * путем отбрасывания exponent младших битов, где exponent - степень
 * размера страницы
 */
int getPageNumberFromVA(VA ptr) {
    int szPage = input->szPage;
    int exponent = (int) (log2((double) szPage));
    int decimalPtr = convertToDecimal(ptr);
    int pageNum = decimalPtr >> exponent;
    return pageNum;
}

/*
 * Получение смещения виртуального адреса
 */
/*VA getVAoffset(VA ptr) {
    int szPage = input->szPage;
    int exponent = (int) (log2((double) szPage));
    int decimalPtr = convertToDecimal(ptr);

    unsigned mask = 0;
    for (unsigned i = 0; i <= exponent; i++)
        mask |= 1 << i;
    VA offset = convertToVA(mask & decimalPtr);
    return offset;
}*/

VA getVAoffset(VA address) {
    int szPage = input->szPage;
    size_t exponent = (size_t) (log2((double) szPage));
    size_t truncLength = strlen(address) - exponent;
    VA addressOffset = calloc(exponent, exponent * sizeof(char));
    strncpy(addressOffset, address + truncLength, exponent);
    return addressOffset;
}

/*
 * Преобразование виртуального адреса в десятичный адрес блока
 */
int convertToBlockAddr(VA addr) {
    int addressOffset = convertToDecimal(getVAoffset(addr));
    int pageNumber = getPageNumberFromVA(addr);

    int blockAddr = table[pageNumber].firstBlockOffset;
    blockAddr += addressOffset / BLOCK_SIZE;
    return blockAddr;
}

int *addressMapping(VA *ptr, size_t blockSize) {
    int lastAddress = blockSize + convertToDecimal(*ptr);
    int *addressPool = getPhysAddrPool(convertToDecimal(*ptr), lastAddress);
    return addressPool;
}

int findBlockAddr(VA ptr) {
    int blockAddress = convertToBlockAddr(ptr);
    return blockAddress;
}

int isPageAvailable(int pageNum) {
    struct pageInfo info = table[pageNum];
    if (info.isAvailable == '1')
        return SUCCESS;
    else return UNKNOWN_MISTAKE;
}

struct page *findPage(int pageNum);

struct page *findOptimalPage(int anchorNum) {

    struct page *optimalPage;
    int optimalPageNum;
    do {
        optimalPageNum = rand() % input->n + 1;
        optimalPage = findPage(optimalPageNum);
    } while (optimalPage == NULL || optimalPageNum == anchorNum);
    return optimalPage;
}

void loadExtraPageToDisk(int anchorPage) {
    struct page *pageToUnload = findOptimalPage(anchorPage);

    struct diskCell *cell = (struct diskCell *) malloc(sizeof(struct diskCell));
    cell->pageOnDisk = *pageToUnload;
    cell->pageOffset = pageToUnload->pageNum;
    cell->next = NULL;
    pushDiskCell(&memDisk, cell);

    table[pageToUnload->pageNum].isAvailable = '0';
    deletePage(&virtualPages, pageToUnload);
}

struct page *findPage(int pageNum) {
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

int loadPageToMem(int pageNum) {
    struct diskCell *cell = findDiskCell(&memDisk, pageNum);

    struct page pageToLoad = cell->pageOnDisk;
    deleteDiskCell(&memDisk, pageNum);

    loadExtraPageToDisk(pageNum);
    int pushingStatus = pushPage(&virtualPages, &pageToLoad);
    if (pushingStatus != SUCCESS)
        return pushingStatus;

    table[pageNum].isAvailable = '1';

}

int freeBlock(int addr) {

    struct block *blockToFree = findBlockNode(addr);
    if (blockToFree->isUsed == '0')
        return UNKNOWN_MISTAKE;
    blockToFree->isUsed = '0';
    if (blockToFree->data != NULL)
        free(blockToFree->data);
    return SUCCESS;
}

int writeToBlock(int blockAddr, void *pBuffer) {
    struct block *blockToWrite = findBlockNode(blockAddr);
    if (blockToWrite == NULL || blockToWrite->isUsed == '0' || blockToWrite->data != NULL)
        return UNKNOWN_MISTAKE;
    blockToWrite->data = pBuffer;
    return SUCCESS;
}

int readFromBlock(int blockAddr, void *pBuffer, size_t size) {
    struct block *blockToRead = findBlockNode(blockAddr);
    if (blockToRead == NULL || blockToRead->isUsed == '0' || blockToRead->data == NULL)
        return UNKNOWN_MISTAKE;

    memcpy(pBuffer, blockToRead->data, size);
    return SUCCESS;
}

void initPool() {
    int poolSize = input->n * input->szPage / BLOCK_SIZE;
    pool = (struct block *) malloc(poolSize * sizeof(struct block));

    pool->next = NULL;
    pool->data = NULL;
    pool->isUsed = '0';
    pool->address = 0;
}

int checkInitArguments(int n, int szPage) {
    if (n < 0 || n > MAX_NUM_OF_PAGES || isSzPageValid(szPage) != SUCCESS)
        return WRONG_ARGUMENTS;
    return SUCCESS;
}

void saveInput(int n, int szPage) {
    input = malloc(sizeof(struct userInput));
    input->n = n;
    input->szPage = szPage;
}

int createBlocks() {
    int poolSize = input->n * input->szPage / BLOCK_SIZE;

    for (int blockCount = 1; blockCount < poolSize; blockCount++) {


        int status = pushBlock(&pool, blockCount);
        if (status != SUCCESS)
            return UNKNOWN_MISTAKE;
    }
    return SUCCESS;
}

int _init(int n, int szPage) {
    int checkInitArgumentsStatus = checkInitArguments(n, szPage);
    if (checkInitArgumentsStatus != SUCCESS)
        return WRONG_ARGUMENTS;

    saveInput(n, szPage);
    initPool();

    int createBlocksStatus = createBlocks();
    if (createBlocksStatus != SUCCESS)
        return createBlocksStatus;

    int pagesInMem = n;
    if (n > MAX_NUM_OF_PAGES_IN_RAM)
        pagesInMem = MAX_NUM_OF_PAGES_IN_RAM;
    virtualPages = (struct page *) malloc(pagesInMem * sizeof(struct page));
    virtualPages->next = NULL;
    virtualPages->firstBlock = findBlockNode(0);

    table = (struct pageInfo *) malloc(n * sizeof(struct pageInfo));
    memDisk = (struct diskCell *) malloc(n * sizeof(struct diskCell));
    memDisk->next = NULL;
    memDisk->pageOffset = -1;

    if (virtualPages == NULL || table == NULL)
        return UNKNOWN_MISTAKE;

    for (int pageNum = 0; pageNum < n; pageNum++) {
        int pageOffset = pageNum * (szPage / BLOCK_SIZE);
        struct page *thePage = createPage(pageOffset, pageNum);

        table[pageNum] = createPageInfo(pageOffset, pageNum);
        if (pageNum + 1 > MAX_NUM_OF_PAGES_IN_RAM) {
            struct diskCell *cell = createDiskCell(*thePage, pageNum);
            int pushingStatus = pushDiskCell(&memDisk, cell);
            if (pushingStatus != SUCCESS)
                return UNKNOWN_MISTAKE;
            continue;
        }

        int pushingStatus = pushPage(&virtualPages, thePage);
        if (pushingStatus != SUCCESS)
            return UNKNOWN_MISTAKE;

    }

    return SUCCESS;
}

int _malloc(VA *ptr, size_t blockSize) {
    int maxMemorySize = input->n * input->szPage;
    if (blockSize <= 0 || isAddressValid(*ptr) == WRONG_ARGUMENTS) {
        return WRONG_ARGUMENTS;
    }
    if (blockSize > maxMemorySize || convertToDecimal(*ptr) + blockSize > maxMemorySize)
        return MEMORY_LACK;

    int blocksToAllocateNum = (int) ceil((double) blockSize / BLOCK_SIZE);

    int firstPageNum = getPageNumberFromVA(*ptr);
    if (blocksToAllocateNum + firstPageNum > input->n)
        return MEMORY_LACK;


    struct page *firstPage = findPage(firstPageNum);
    int *addressesToAlloc = addressMapping(ptr, blockSize);
    struct block *firstBlock = firstPage->firstBlock;
    for (int count = 0; count < blocksToAllocateNum; count++) {
        int addrToAlloc = *(addressesToAlloc + count);
        int allocationRes = allocateBlock(&firstBlock, addrToAlloc);
        if (allocationRes != SUCCESS)
            return UNKNOWN_MISTAKE;
    }
    return SUCCESS;
}

int _free(VA ptr) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS)
        return WRONG_ARGUMENTS;

    int blockAddr = findBlockAddr(ptr);
    int freeRes = freeBlock(blockAddr);
    if (freeRes == UNKNOWN_MISTAKE)
        return UNKNOWN_MISTAKE;
    return SUCCESS;

}

int _write(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;
    if (szBuffer > BLOCK_SIZE) {
        return MEMORY_LACK;
    }
    int pageIndex = getPageNumberFromVA(ptr);
    if (isPageAvailable(pageIndex) != SUCCESS) {
        loadPageToMem(pageIndex);
    }
    int blockAddr = findBlockAddr(ptr);
    return writeToBlock(blockAddr, pBuffer);
}

int _read(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;
    if (szBuffer > BLOCK_SIZE) {
        return MEMORY_LACK;
    }
    int pageIndex = getPageNumberFromVA(ptr);
    if (isPageAvailable(pageIndex) != SUCCESS) {
        loadPageToMem(pageIndex);
    }
    int blockAddr = findBlockAddr(ptr);
    return readFromBlock(blockAddr, pBuffer, szBuffer);
}
