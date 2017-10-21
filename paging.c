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
    nextBlock->usedSize = 0;
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

int allocateBlock(struct block *firstBlock) {

    if (firstBlock->isUsed == '1' && firstBlock->usedSize == firstBlock->blockSize)
        return MEMORY_LACK;
    firstBlock->data = calloc((size_t) input->szPage, sizeof(char) * input->szPage);
    firstBlock->isUsed = '1';
    firstBlock->usedSize += 1;
    return SUCCESS;

}

int allocateBlockOnDiskedPage(int pageNum) {
    struct diskCell *cell = findDiskCell(&memDisk, pageNum);
    if (cell == NULL)
        return UNKNOWN_MISTAKE;

    struct page pageToLoad = cell->pageOnDisk;
    struct block *firstBlock = pageToLoad.firstBlock;
    return allocateBlock(firstBlock);
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
    struct block *current = memory;
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

    newPage->next = NULL;
    newPage->pageNum = pageNum;
    newPage->freeSize = input->szPage;
    newPage->firstBlock = NULL;

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

int mapPageWithBlock(int pageNum) {
    struct page *newPage = getPage(pageNum);
    if (newPage->firstBlock != NULL) {
        return SUCCESS;
    }
    int physBlockAddr = findFreeBlock()->address;
    table[pageNum].physBlockAddr = physBlockAddr;
    table[pageNum].isAvailable = '1';

    struct block *firstBlock = findBlockNode(physBlockAddr);
    int status = allocateBlock(firstBlock);
    if (status == SUCCESS) {
        newPage->firstBlock = firstBlock;
    }
    return status;

}

struct diskCell *createDiskCell(struct page thePage, int pageNum) {
    struct diskCell *cell = (struct diskCell *) malloc(sizeof(struct diskCell));
    cell->pageOnDisk = thePage;
    cell->usedSize = 0;
    cell->data= NULL;
    cell->pageNum = pageNum;
    cell->next = NULL;
    return cell;
}

/*
 * Создание "строки" в таблице страниц
 */
struct pageInfo createPageInfo(int virtualPageNum) {
    struct pageInfo *info = (struct pageInfo *) malloc(sizeof(struct pageInfo));


    /*if (virtualPageNum > MAX_NUM_OF_PAGES_IN_RAM) {
        info->isAvailable = '0';
        info->physBlockAddr = virtualPageNum * input->system_constant;
    } else {
        info->isAvailable = '1';
        info->physBlockAddr = -1;
    }*/
    info->isAvailable = '0';
    info->physBlockAddr = -1;
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
    if (szPage <= 0)
        return WRONG_ARGUMENTS;
    double exponent = log2((double) szPage);
    if (exponent - ceil(exponent) == 0)
        return SUCCESS;
    else return WRONG_ARGUMENTS;
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
 * Преобразование виртуального адреса в адрес блока
 */
int convertToBlockAddr(VA addr) {
    int pageNumber = getPageNumberFromVA(addr);

    int blockAddr = table[pageNumber].physBlockAddr * input->system_constant;
    return blockAddr;
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

int isPageInMem(int pageNum) {
    struct page *current = virtualPages->next;
    if (current->pageNum == pageNum && current->next == NULL)
        return SUCCESS;
    int pageInMemCount = 0;
    while (pageInMemCount < MAX_NUM_OF_PAGES_IN_RAM && current->next!=NULL) {
        if (table[current->pageNum].isAvailable == '1') {

            pageInMemCount++;
            if (current->pageNum == pageNum)
                return SUCCESS;
        }

        current = current->next;
    };
    if (current->pageNum == pageNum)
        return SUCCESS;
    return UNKNOWN_MISTAKE;
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

void freeBlock(struct block *physBlock) {

    physBlock->isUsed = '0';
    physBlock->usedSize = 0;
    if (physBlock->data != "")
        free(physBlock->data);
}

void loadPageFromMemToDisk(int anchorPage) {
    struct page *pageToUnload = findOptimalPage(anchorPage);
    struct block *firstBlock = pageToUnload->firstBlock;
    int pageNum = pageToUnload->pageNum;

    struct diskCell *cell = findDiskCell(&memDisk, pageNum);
    memcpy(cell->data, firstBlock->data, (size_t) firstBlock->usedSize);
    cell->usedSize = firstBlock->usedSize;
    table[pageNum].isAvailable = '0';
    table[pageNum].physBlockAddr = -1;
    freeBlock(firstBlock);

}

struct page *findPageInMem(int pageNum) {
    struct page *current = virtualPages->next;
    if (current->pageNum == pageNum && current->next == NULL)
        return current;
    int pageInMemCount = 0;
    while (pageInMemCount < MAX_NUM_OF_PAGES_IN_RAM) {
        if (table[current->pageNum].isAvailable == '1') {

            pageInMemCount++;
            if (current->pageNum == pageNum)
                return current;
        }

        current = current->next;
    };
    if (current->pageNum == pageNum)
        return current;
    return NULL;
}

int loadPageToMem(int pageNum) {
    printf("loading page to mem\n");
    struct diskCell *cell = findDiskCell(&memDisk, pageNum);
    if (cell == NULL)
        return UNKNOWN_MISTAKE;

    struct page pageToLoad = cell->pageOnDisk;
    // deleteDiskCell(&memDisk, pageNum);

    struct block *freeBlock = findFreeBlock();
    if (freeBlock == NULL) {
        loadPageFromMemToDisk(pageNum);
        freeBlock = findFreeBlock();
    }
    (&pageToLoad)->firstBlock = freeBlock;

    /*int pushingStatus = pushPage(&virtualPages, &pageToLoad);
    if (pushingStatus != SUCCESS)
        return pushingStatus;*/

    table[pageNum].isAvailable = '1';
    table[pageNum].physBlockAddr = freeBlock->address;

}

int freeBlocks(struct page *firstPage) {
    struct page *current = firstPage;
    if (current->firstBlock->isUsed == '0')
        return UNKNOWN_MISTAKE;
    //if (current->freeSize != input->szPage) {
    struct block *physBlock = current->firstBlock;
    if (physBlock->isUsed == '0')

        freeBlock(physBlock);
    //}
    return SUCCESS;
}

int freePages(struct page *firstPage) {
    struct page *current = firstPage;
    while (current->next != NULL) {
        struct block *physBlock = current->firstBlock;
        if (physBlock->isUsed == '0')
            break;
        if (physBlock->usedSize == physBlock->blockSize) {
            physBlock->isUsed = '0';
            physBlock->usedSize = 0;
            if (physBlock->data != "")
                free(physBlock->data);
        } else {

        }
        //}
        current = current->next;
    }
    return SUCCESS;
}

int freeBlocksOnDiskedPage(int pageNum) {
    struct diskCell *cell = findDiskCell(&memDisk, pageNum);
    if (cell == NULL)
        return UNKNOWN_MISTAKE;

    struct page *pageToLoad = &(cell->pageOnDisk);

    return freeBlocks(pageToLoad);
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
    if (blockToRead == NULL || blockToRead->isUsed == '0' || blockToRead->data == NULL)
        return UNKNOWN_MISTAKE;
    char *dataValue = calloc(1, sizeof(char));
    dataValue[0] = blockToRead->data[offset];
    memcpy(pBuffer, dataValue, 1);
    return SUCCESS;
}

void initPhysMem() {
    int memSize = (MAX_NUM_OF_PAGES_IN_RAM * MAX_PAGE_SIZE) / input->szPage;
    memory = (struct block *) malloc(memSize * sizeof(struct block));

    memory->next = NULL;
    memory->writeStatus = '0';
    memory->blockSize = input->szPage;
    memory->data = NULL;
    memory->isUsed = '0';
    memory->address = 0;
    memory->usedSize = 0;
}

int checkInitArguments(int n, int szPage) {
    if (n < 0 || isSzPageValid(szPage) != SUCCESS)
        return WRONG_ARGUMENTS;
    if (n > MAX_NUM_OF_PAGES || szPage > MAX_PAGE_SIZE || n * szPage > MAX_SIZE)
        return MEMORY_LACK;
    return SUCCESS;
}

void saveInput(int n, int szPage) {
    input = malloc(sizeof(struct userInput));
    input->n = n;
    input->szPage = szPage;
    input->system_constant = 1;
}

int createBlocks() {
    int poolSize = MAX_NUM_OF_PAGES_IN_RAM;

    for (int blockCount = 1; blockCount < poolSize; blockCount++) {


        int status = pushBlock(&memory, blockCount, input->szPage);
        if (status != SUCCESS)
            return UNKNOWN_MISTAKE;
    }
    return SUCCESS;
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

        table[pageNum] = createPageInfo(pageNum);

        pushingStatus = pushPage(&virtualPages, thePage);
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
        int allocStatus = mapPageWithBlock(currPageNum);

        if (allocStatus != SUCCESS)
            return allocStatus;

    }
    return SUCCESS;
}

int _free(VA ptr) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS)
        return WRONG_ARGUMENTS;

    int pageNum = getPageNumberFromVA(ptr);

    if (isPageInMem(pageNum) != SUCCESS) {
        return freeBlocksOnDiskedPage(pageNum);
    } else {
        struct page *currPage = findPageInMem(pageNum);
        int freeStatus = freeBlocks(currPage);

        return freeStatus;
    }

}

int _write(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;

    int firstAddr = convertToDecimal(ptr);
    int lastAddr = firstAddr + szBuffer;

    if (szBuffer > input->n * input->szPage || lastAddr > input->n * input->szPage) {
        return MEMORY_LACK;
    }

    char *data = (char *) pBuffer;
    for (int i = firstAddr; i < lastAddr; i++) {
        VA address = convertToVA(i);
        int currPageNum = getPageNumberFromVA(address);
        if (isPageInMem(currPageNum) != SUCCESS) {
            loadPageToMem(currPageNum);
        }

        struct page *currPage = findPageInMem(currPageNum);
        if (currPage == NULL) {
            return UNKNOWN_MISTAKE;
        }

        int offset = convertToDecimal(getVAoffset(address));
        char dataChar = data[i - firstAddr];
        int writeStatus = writeInPage(currPage, dataChar, offset);
        if (writeStatus != SUCCESS)
            return writeStatus;
        currPage->freeSize -= 1;
    }

    return SUCCESS;
}

int _read(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;

    int firstAddr = convertToDecimal(ptr);
    int lastAddr = firstAddr + szBuffer;

    if (szBuffer > input->n * input->szPage || lastAddr > input->n * input->szPage) {
        return MEMORY_LACK;
    }

    for (int i = firstAddr; i < lastAddr; i++) {
        VA address = convertToVA(i);
        int currPageNum = getPageNumberFromVA(address);
        if (isPageInMem(currPageNum) != SUCCESS) {
            loadPageToMem(currPageNum);
        }
        struct page *currPage = findPageInMem(currPageNum);
        if (currPage == NULL) {
            return UNKNOWN_MISTAKE;
        }

        int offset = convertToDecimal(getVAoffset(address));
        char *data = (char *) malloc(sizeof(char));
        int readStatus = readFromPage(currPage, data, offset);
        if (readStatus != SUCCESS)
            return readStatus;
        strcat(pBuffer, data);
    }

    return SUCCESS;
}
