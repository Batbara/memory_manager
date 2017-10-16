#include "mmemory.h"
#include "paging.h"

#include <malloc.h>
#include <mem.h>
#include <math.h>
#include <stdio.h>

/*
 * Добавление блока в связный список
 */
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
    nextBlock->data = NULL;
    nextBlock->isUsed = '0';
    nextBlock->next = NULL;
    nextBlock->writeStatus = '0';

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

void deleteDiskCell(struct diskCell **head, int pageOffset) {
    struct diskCell *current = *head;
    struct diskCell *prev;

    if (current != NULL && current->pageNum == pageOffset) {
        *head = current->next;
        free(current);
        return;
    }

    while (current != NULL && current->pageNum != pageOffset) {
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

int allocateBlock(struct block *firstBlock, int freeSize) {

    if (firstBlock->isUsed == '1' && freeSize == input->szPage)
        return MEMORY_LACK;
    firstBlock->data = calloc((size_t) input->szPage, sizeof(char) * input->szPage);
    firstBlock->isUsed = '1';
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
struct page *createPage(int pageNum) {

    struct page *newPage = (struct page *) malloc(sizeof(struct page));

    int physBlockAddr = table[pageNum].physBlockAddr;

    struct block *firstBlock = findBlockNode(physBlockAddr);

    newPage->firstBlock = firstBlock;
    newPage->next = NULL;
    newPage->pageNum = pageNum;
    newPage->freeSize = input->szPage;

    return newPage;
}

struct diskCell *createDiskCell(struct page thePage, int pageNum) {
    struct diskCell *cell = (struct diskCell *) malloc(sizeof(struct diskCell));
    cell->pageOnDisk = thePage;
    cell->pageNum = pageNum;
    cell->next = NULL;
    return cell;
}

/*
 * Создание "строки" в таблице страниц
 */
struct pageInfo createPageInfo(int virtualPageNum) {
    struct pageInfo *info = (struct pageInfo *) malloc(sizeof(struct pageInfo));
    info->physBlockAddr = virtualPageNum * input->system_constant;

    if (virtualPageNum > MAX_NUM_OF_PAGES_IN_RAM) {
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
        addrPool[count] = findBlockAddr(convertToVA(addr));
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

    int blockAddr = table[pageNumber].physBlockAddr * input->system_constant;
    // blockAddr += addressOffset;
    return blockAddr;
}

int *VApool(VA *ptr, size_t blockSize) {
    int lastAddress = blockSize + convertToDecimal(*ptr);
    int firstAddr = convertToDecimal(*ptr);
//    int lastPhysAddress = findBlockAddr(convertToVA(lastAddress));
//    int firstPhysAddress = findBlockAddr(*ptr);
    //int *addressPool = getPhysAddrPool( convertToDecimal(*ptr), lastAddress);
    int *addrPool = malloc(blockSize * sizeof(int));
    for (int i = 0; i < blockSize; i++) {
        *(addrPool + i) = firstAddr + i;
    }
    return addrPool;
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

struct page *findPageInMem(int pageNum);

struct page *findOptimalPage(int anchorNum) {

    struct page *optimalPage;
    int optimalPageNum;
    do {
        optimalPageNum = rand() % input->n + 1;
        optimalPage = findPageInMem(optimalPageNum);
    } while (optimalPage == NULL || optimalPageNum == anchorNum);
    return optimalPage;
}

void loadExtraPageToDisk(int anchorPage) {
    struct page *pageToUnload = findOptimalPage(anchorPage);

    struct diskCell *cell = (struct diskCell *) malloc(sizeof(struct diskCell));
    cell->pageOnDisk = *pageToUnload;
    cell->pageNum = pageToUnload->pageNum;
    cell->next = NULL;
    pushDiskCell(&memDisk, cell);

    table[pageToUnload->pageNum].isAvailable = '0';
    deletePage(&virtualPages, pageToUnload);
}

struct page *findPageInMem(int pageNum) {
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
    if (cell == NULL)
        return UNKNOWN_MISTAKE;

    struct page pageToLoad = cell->pageOnDisk;
    deleteDiskCell(&memDisk, pageNum);

    loadExtraPageToDisk(pageNum);
    int pushingStatus = pushPage(&virtualPages, &pageToLoad);
    if (pushingStatus != SUCCESS)
        return pushingStatus;

    table[pageNum].isAvailable = '1';

}

int freeBlocks(struct page ** firstPage) {
    struct page * current = *firstPage;
    while(current->next!=NULL){
        if(current->freeSize != input->szPage){
            struct block *physBlock = current->firstBlock;
            if (physBlock->isUsed == '0')
                return UNKNOWN_MISTAKE;
            physBlock->isUsed = '0';
            if (physBlock->data != NULL)
                free(physBlock->data);
        }
        current = current->next;
    }
    return SUCCESS;
}

int writeInPage(struct page * currPage, char inputData, int position) {
    struct block *blockToWrite = currPage->firstBlock;
    if (blockToWrite == NULL || blockToWrite->writeStatus == '1' || blockToWrite->data == NULL)
        return UNKNOWN_MISTAKE;
    blockToWrite->data[position] = inputData;

   // blockToWrite->writeStatus = '1';
    return SUCCESS;
}

int readFromBlock(struct page * currPage, void *pBuffer, int offset, size_t size) {
    struct block *blockToRead = currPage->firstBlock;
    if (blockToRead == NULL || blockToRead->isUsed == '0' || blockToRead->data == NULL)
        return UNKNOWN_MISTAKE;
    char *dataValue = calloc(size, size * sizeof(char));
    for (int i = offset; i < size; i++) {
        dataValue[i] = blockToRead->data[i];
    }

    memcpy(pBuffer, dataValue, size);
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
    input->system_constant = 1;/*(int) log2((double) szPage);*/
}

int createBlocks() {
    int poolSize = MAX_NUM_OF_PAGES_IN_RAM * input->szPage;

    for (int blockCount = 1; blockCount < poolSize; blockCount++) {


        int status = pushBlock(&pool, blockCount, input->szPage);
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
    memDisk->pageNum = -1;

    if (virtualPages == NULL || table == NULL)
        return UNKNOWN_MISTAKE;

    for (int pageNum = 0; pageNum < n; pageNum++) {

        table[pageNum] = createPageInfo(pageNum);
        struct page *thePage = createPage(pageNum);

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
    int firstAddr = convertToDecimal(*ptr);
    int lastAddr = firstAddr + blockSize;
    for (int i = firstAddr; i < lastAddr; i++) {
        VA address = convertToVA(i);
        int currPageNum = getPageNumberFromVA(address);
        if (currPageNum > MAX_NUM_OF_PAGES_IN_RAM) {
            loadPageToMem(currPageNum);
        }
        struct page *currPage = findPageInMem(currPageNum);
        if (currPage == NULL) {
            return UNKNOWN_MISTAKE;
        }
        currPage->freeSize -= 1;

        int allocStatus = allocateBlock(currPage->firstBlock, currPage->freeSize);
        if (allocStatus != SUCCESS)
            return allocStatus;

    }
    return SUCCESS;
}

int _free(VA ptr) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS)
        return WRONG_ARGUMENTS;

    int pageNum = getPageNumberFromVA(ptr);
    if (pageNum > MAX_NUM_OF_PAGES_IN_RAM) {
        loadPageToMem(pageNum);
    }
    struct page *currPage = findPageInMem(pageNum);
    int freeStatus = freeBlocks(&currPage);

    return freeStatus;

}

int _write(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;

    int firstAddr = convertToDecimal(ptr);
    int lastAddr = firstAddr + szBuffer;

    if (szBuffer > input->n * input->szPage || lastAddr>input->n*input->szPage) {
        return MEMORY_LACK;
    }

    char* data = (char*) pBuffer;
    for (int i = firstAddr; i < lastAddr; i++) {
        VA address = convertToVA(i);
        int currPageNum = getPageNumberFromVA(address);
        if (currPageNum > MAX_NUM_OF_PAGES_IN_RAM) {
            loadPageToMem(currPageNum);
        }
        struct page *currPage = findPageInMem(currPageNum);
        if (currPage == NULL) {
            return UNKNOWN_MISTAKE;
        }

        int offset = convertToDecimal(getVAoffset(address));
        char dataChar = data[i-firstAddr];
        int writeStatus = writeInPage(currPage,dataChar,offset);
        if(writeStatus!= SUCCESS)
            return writeStatus;
    }

    return SUCCESS;
}

int _read(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;

    int firstAddr = convertToDecimal(ptr);
    int lastAddr = firstAddr + szBuffer;

    if (szBuffer > input->n * input->szPage || lastAddr>input->n*input->szPage) {
        return MEMORY_LACK;
    }
    char* data = (char*)pBuffer;
    for (int i = firstAddr; i < lastAddr; i++) {
        VA address = convertToVA(i);
        int currPageNum = getPageNumberFromVA(address);
        if (currPageNum > MAX_NUM_OF_PAGES_IN_RAM) {
            loadPageToMem(currPageNum);
        }
        struct page *currPage = findPageInMem(currPageNum);
        if (currPage == NULL) {
            return UNKNOWN_MISTAKE;
        }

        int offset = convertToDecimal(getVAoffset(address));
        int size = input->n - currPage->freeSize;
        data = (char*)malloc(sizeof(char)*size);
        int readStatus = readFromBlock(currPage,data,offset,size);
        if(readStatus!= SUCCESS)
            return readStatus;
        strcat(pBuffer, data);
    }

    return SUCCESS;
}
