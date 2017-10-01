#include "mmemory.h"
#include "paging.h"

#include <malloc.h>
#include <mem.h>
#include <math.h>
#include <stdio.h>

/*
 * Добавление блока в связный список
 */
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

/*
 * Получние ссылки на блок по исходному виртуальному адресу
 */
struct block *getBlockByAddress(struct block **head, VA address) {
    struct block *current = *head;
    while (current != NULL) {
        char *blockAddress = current->address;
        if (strcmp(blockAddress, address) == 0)
            return current;
        current = current->next;
    }
    return NULL;
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
    struct diskCell *temp = *head;
    struct diskCell *prev;

    if (temp != NULL && temp->pageOffset == pageOffset) {
        *head = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && temp->pageOffset != pageOffset) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;

    prev->next = temp->next;

    free(temp);
}

void deletePage(struct page **head, struct page *pageToDel) {
    struct page *temp = *head;
    struct page *prev;

    if (temp != NULL && temp == pageToDel) {
        *head = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && temp != pageToDel) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;

    prev->next = temp->next;

    free(temp);
}

int allocateBlock(struct block **firstBlock, VA addrToAlloc) {
    struct block *current = *firstBlock;
    while (current->next != NULL) {
        VA blockAddress = current->address;
        if (strcmp(blockAddress, addrToAlloc) == 0) {
            if (current->isUsed == '1')
                return UNKNOWN_MISTAKE;
            current->isUsed = '1';
            return SUCCESS;
        }
        current = current->next;
    }
    return SUCCESS;
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

/*
 * Создание виртуальной страницы
 */
struct page *createPage(int pageOffset, struct block **memoryPool, int pageNum) {

    struct page *newPage = (struct page *) malloc(sizeof(struct page));

    VA firstBlockAddress = convertToVA(pageOffset);
    struct block *firstBlock = getBlockByAddress(memoryPool, firstBlockAddress);

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
    info->addrOffset = pageOffset;

    if (pageOffset > MAX_NUM_OF_PAGES_IN_RAM) {
        info->isAvailable = '0';
    } else
        info->isAvailable = '1';
    info->isModified = '0';
    info->accessCounter = 0;
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
VA *getVApool(VA firstVA, VA lastVA) {
    int size = convertToDecimal(lastVA) - convertToDecimal(firstVA);
    VA *VApool = (VA *) malloc(size * sizeof(VA));
    for (int addr = convertToDecimal(firstVA), count = 0; addr < convertToDecimal(lastVA); addr++, count++) {
        VApool[count] = convertToVA(addr);
    }
    return VApool;
}

/*
 * Получение номера виртуальной страницы из виртуального адреса
 * путем отбрасывания exponent младших битов, где exponent - степень
 * размера страницы
 */
int getPageNumberFromVA(VA address) {
    int szPage = input->szPage;
    int exponent = (int) (log2((double) szPage));
    int decimalAddress = convertToDecimal(address);
    int pageNum = decimalAddress >> exponent;
    return pageNum;
}

/*
 * Получение смещения виртуального адреса
 */
VA getVAoffset(VA address) {
    int szPage = input->szPage;
    int exponent = (int) (log2((double) szPage));
    int decimalAddress = convertToDecimal(address);

    unsigned mask = 0;
    for (unsigned i = 0; i <= exponent; i++)
        mask |= 1 << i;
    return convertToVA(mask & decimalAddress);
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

VA *addressMapping(VA *ptr, size_t blockSize) {
    int lastAddrDecimal = blockSize + convertToDecimal(*ptr);
    VA lastAddress = convertToVA(lastAddrDecimal);
    VA *VApool = getVApool(*ptr, lastAddress);
    //   VA *addressesToAlloc = getVAofBlocksToAlloc(blockSize, VApool, blockSize);
    return VApool;
}

VA findBlockAddr(VA ptr) {
    VA blockAddress = convertToVA(convertToBlockAddr(ptr));
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

    int optimalPageNum = 0;
    if (anchorNum == 0)
        if (input->n > MAX_NUM_OF_PAGES_IN_RAM)
            optimalPageNum = MAX_NUM_OF_PAGES_IN_RAM;
        else optimalPageNum = input->n;
    table[optimalPageNum].isAvailable = '0';
    printf("unloading to disk page number %d\n", optimalPageNum);
    return findPage(optimalPageNum);
}

void loadExtraPageToDisk(int anchorPage) {
    struct page *pageToUnload = findOptimalPage(anchorPage);
    deletePage(&virtualPages, pageToUnload);
}

struct page *findPage(int pageNum) {
    struct page *current = virtualPages;
    while (current->next != NULL) {
        if (current->pageNum == pageNum)
            return current;
        current = current->next;
    };
    return current;
}

int loadPageToMem(int pageNum) {
    printf("loading to memory page number%d\n", pageNum);
    struct diskCell *cell = findDiskCell(&memDisk, pageNum);

    struct page pageToLoad = cell->pageOnDisk;
    deleteDiskCell(&memDisk, pageNum);

    loadExtraPageToDisk(pageNum);
    int pushingStatus = pushPage(&virtualPages, &pageToLoad);
    if (pushingStatus != SUCCESS)
        return pushingStatus;

    table[pageNum].isAvailable = '1';

}

struct block *findBlockNode(VA blockAddr) {
    struct block *current = pool;
    while (current->next != NULL) {
        if (strcmp(blockAddr, current->address) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int freeBlock(VA addr) {

    struct block *blockToFree = findBlockNode(addr);
    if (blockToFree->isUsed == '0')
        return UNKNOWN_MISTAKE;
    blockToFree->isUsed = '0';
    if (blockToFree->data != NULL)
        free(blockToFree->data);
    return SUCCESS;
}

int writeToBlock(VA blockAddr, void *pBuffer) {
    struct block *blockToWrite = findBlockNode(blockAddr);
    if (blockToWrite == NULL || blockToWrite->isUsed == '0' || blockToWrite->data != NULL)
        return UNKNOWN_MISTAKE;
    blockToWrite->data = pBuffer;
    blockToWrite->isUsed = '1';
    return SUCCESS;
}

int readFromBlock(VA blockAddr, void *pBuffer, size_t size) {
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
    pool->blockSize = BLOCK_SIZE;
    pool->address = convertToVA(0);
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
        VA address = convertToVA(blockCount);

        int status = pushBlock(&pool, address, BLOCK_SIZE);
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
    virtualPages->firstBlock = getBlockByAddress(&pool, convertToVA(0));

    table = (struct pageInfo *) malloc(n * sizeof(struct pageInfo));
    memDisk = (struct diskCell *) malloc(n * sizeof(struct diskCell));
    memDisk->next = NULL;
    memDisk->pageOffset = -1;

    if (virtualPages == NULL || table == NULL)
        return UNKNOWN_MISTAKE;

    for (int pageNum = 0; pageNum < n; pageNum++) {
        int pageOffset = pageNum * (szPage / BLOCK_SIZE);
        struct page *thePage = createPage(pageOffset, &pool, pageNum);

        if (pageNum + 1 > MAX_NUM_OF_PAGES_IN_RAM) {
            struct diskCell *cell = createDiskCell(*thePage, pageNum);
            int pushingStatus = pushDiskCell(&memDisk, cell);
            if (pushingStatus != SUCCESS)
                return UNKNOWN_MISTAKE;
            continue;
        }

        //  int pushingStatus = pushPage(&virtualPages, pageOffset, &pool);
        int pushingStatus = pushPage(&virtualPages, thePage);
        if (pushingStatus != SUCCESS)
            return UNKNOWN_MISTAKE;

        table[pageNum] = createPageInfo(pageOffset, pageNum);
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

    int pagesToAllocateNum = (int) ceil((double) blockSize / input->szPage);
    int firstPageNum = getPageNumberFromVA(*ptr);
    if (blocksToAllocateNum + firstPageNum > input->n)
        return MEMORY_LACK;

    for (int pageIndex = firstPageNum; pageIndex < firstPageNum + pagesToAllocateNum; pageIndex++) {
        if (isPageAvailable(pageIndex) != SUCCESS) {
            loadPageToMem(pageIndex);
        }
    }

    struct page *firstPage = findPage(firstPageNum);
    VA *addressesToAlloc = addressMapping(ptr, blockSize);
    struct block *firstBlock = firstPage->firstBlock;
    for (int count = 0; count < blocksToAllocateNum; count++) {
        VA addrToAlloc = *(addressesToAlloc + count);
        int allocationRes = allocateBlock(&firstBlock, addrToAlloc);
        if (allocationRes != SUCCESS)
            return UNKNOWN_MISTAKE;
    }
    return SUCCESS;
}

int _free(VA ptr) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS)
        return WRONG_ARGUMENTS;

    VA blockAddr = findBlockAddr(ptr);
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
    int pageNum = getPageNumberFromVA(ptr);
    table[pageNum].isModified = '1';
    table[pageNum].accessCounter++;

    VA blockAddr = findBlockAddr(ptr);
    return writeToBlock(blockAddr, pBuffer);
}

int _read(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;
    if (szBuffer > BLOCK_SIZE) {
        return MEMORY_LACK;
    }
    int pageNum = getPageNumberFromVA(ptr);
    table[pageNum].isModified = '1';
    table[pageNum].accessCounter++;
    VA blockAddr = findBlockAddr(ptr);
    return readFromBlock(blockAddr, pBuffer, szBuffer);
}
