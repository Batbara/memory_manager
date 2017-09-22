#include "mmemory.h"
#include "paging.h"

#include <malloc.h>
#include <mem.h>
#include <math.h>

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

/*
 * Резервирование блоков - отметка флага isUsed в 1
 */
int allocateBlocks(struct block **firstBlock, VA *addressesToAlloc, int size) {
    struct block *current = *firstBlock;
    while (current->next != NULL) {
        VA blockAddress = current->address;
        for (int count = 0; count < size; count++) {
            VA addrToAlloc = *(addressesToAlloc + count);
            if (strcmp(blockAddress, addrToAlloc) == 0) {
                if (current->isUsed == '1')
                    return UNKNOWN_MISTAKE;
                current->isUsed = '1';
                break;
            }
        }
        current = current->next;
    }
    return SUCCESS;
}

/*
 * Преобразование десятичного номера блока в двоичный адрес
 */
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

/*
 * Преборазование виртуального адреса в десятичнй номер
 */
int convertToDecimal(VA addressToConvert) {
    int decimal = 0;
    for (int bit = strlen(addressToConvert) - 1, exp = 0; bit >= 0; bit--, exp++) {
        int number = addressToConvert[bit] - '0';
        decimal += number * pow(2, exp);
    }
    return decimal;
}

/*
 * Создание виртуальной страницы
 */
struct page createPage(int pageOffset, struct block **memoryPool, int pageNum) {

    struct page *newPage = (struct page *) malloc(sizeof(struct page));

    VA firstBlockAddress = convertToVA(pageOffset);
    struct block *firstBlock = getBlockByAddress(memoryPool, firstBlockAddress);

    newPage->firstBlock = firstBlock;
    newPage->pageNum = pageNum;

    return *newPage;
}

/*
 * Создание "строки" в таблице страниц
 */
struct pageInfo createPageInfo(int pageOffset) {
    struct pageInfo *info = (struct pageInfo *) malloc(sizeof(struct pageInfo));
    info->firstBlockOffset = pageOffset;
    info->isAvailable = '1';
    return *info;
}

/*
 * Проверка корректности виртуального адреса
 */
int isAddressValid(VA ptr) {
    if (strlen(ptr) != ADDRESS_CAPACITY + 1)
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
    VA *VApool = malloc(size * sizeof(VA));
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
int retrievePageNumberFromVA(VA address) {
    int szPage = input->szPage;
    int exponent = (int) (log2((double) szPage));
    size_t truncLength = strlen(address) - exponent;
    VA pageNumInBinary = calloc(truncLength, truncLength * sizeof(char));
    strncpy(pageNumInBinary, address, truncLength);

    int pageNum = convertToDecimal(pageNumInBinary);
    return pageNum;
}

/*
 * Получение смещения виртуального адреса
 */
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
    int pageNumber = retrievePageNumberFromVA(addr);

    int blockAddr = table[pageNumber].firstBlockOffset;
    blockAddr += addressOffset / BLOCK_SIZE;
    return blockAddr;
}

/*
 * Получение виртуальных адресов блоков, которые выделяются в памяти
 */
VA *getVAofBlocksToAlloc(int blockSize, VA *pagingVApool, int poolSize) {
    int blocksNum = (int) ceil((double) blockSize / BLOCK_SIZE);
    VA *blockAddrPool = malloc(blocksNum * sizeof(VA));
    int prevAddr = -1, poolCount = 0;
    for (int count = 0; count < poolSize; count++) {
        VA currentVA = pagingVApool[count];
        int blockAddr = convertToBlockAddr(currentVA);

        if (blockAddr == prevAddr)
            continue;
        prevAddr = blockAddr;
        blockAddrPool[poolCount] = convertToVA(blockAddr);
        poolCount++;
    }
    return blockAddrPool;

}

VA *addressMapping(VA *ptr, size_t blockSize) {
    int lastAddrDecimal = blockSize + convertToDecimal(*ptr);
    VA lastAddress = convertToVA(lastAddrDecimal);
    VA *VApool = getVApool(*ptr, lastAddress);
    VA *addressesToAlloc = getVAofBlocksToAlloc(blockSize, VApool, blockSize);
    return addressesToAlloc;
}

VA findBlockAddr(VA ptr) {
    VA blockAddress = convertToVA(convertToBlockAddr(ptr));
    return blockAddress;
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

int _init(int n, int szPage) {
    if (n < 1 || n > MAX_NUM_OF_PAGES || isSzPageValid(szPage) != SUCCESS)
        return WRONG_ARGUMENTS;
    input = malloc(sizeof(struct userInput));
    input->n = n;
    input->szPage = szPage;

    int poolSize = n * szPage / BLOCK_SIZE;
    pool = (struct block *) malloc(poolSize * sizeof(struct block));
    if (pool == NULL)
        return UNKNOWN_MISTAKE;
    pool->next = NULL;
    pool->data = NULL;
    pool->isUsed = '0';
    pool->blockSize = BLOCK_SIZE;
    pool->address = convertToVA(0);
    for (int blockCount = 1; blockCount < poolSize; blockCount++) {
        VA address = convertToVA(blockCount);

        int status = pushBlock(&pool, address, BLOCK_SIZE);
        if (status != SUCCESS)
            return UNKNOWN_MISTAKE;
    }
    virtualPages = (struct page *) malloc(n * sizeof(struct page));
    table = (struct pageInfo *) malloc(n * sizeof(struct pageInfo));

    if (virtualPages == NULL || table == NULL)
        return UNKNOWN_MISTAKE;

    for (int pageNum = 0; pageNum < n; pageNum++) {
        int pageOffset = pageNum * (szPage / BLOCK_SIZE);
        virtualPages[pageNum] = createPage(pageOffset, &pool, pageNum);
        table[pageNum] = createPageInfo(pageOffset);
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

    /*
     * алгоритм:
     * 1) вычисляем номер страницы с начальным адресом таким адресом
     * 2) считаем конечный адрес блока
     * 3) выводим ошибку, если не хватит памяти
     * 4) устанавливаем флаги занятости
     */

    int blocksToAllocateNum = (int) ceil((double) blockSize / BLOCK_SIZE);
    int firstPageNum = retrievePageNumberFromVA(*ptr);
    if (blocksToAllocateNum + firstPageNum > input->n)
        return MEMORY_LACK;

    struct page firstPage = virtualPages[firstPageNum];
    VA *addressesToAlloc = addressMapping(ptr, blockSize);

    struct block *firstBlock = firstPage.firstBlock;
    int allocationRes = allocateBlocks(&firstBlock, addressesToAlloc, blocksToAllocateNum);
    if (allocationRes != SUCCESS)
        return UNKNOWN_MISTAKE;
    return SUCCESS;
}

int _free(VA ptr) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS)
        return WRONG_ARGUMENTS;
    /*
     * Алгоритм:
     * 1) ищем блок с таким адресом
     * 2) если блок уже свободен, возвращаем UNKNOWN_MISTAKE
     * 3) если блок занят, устанавливаем флаг 'свободен'
     */
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
    /*
     * Алгоритм:
     * 0) проверяем корректность парам
     * 1) определяем блок с таким адресом
     * 2) если он не замаллочен или есть данные - неизвестная ошибка
     * 3) если все ок, пишем в блок
     */
    VA blockAddr = findBlockAddr(ptr);
    return writeToBlock(blockAddr, pBuffer);
}

int _read(VA ptr, void *pBuffer, size_t szBuffer) {
    if (isAddressValid(ptr) == WRONG_ARGUMENTS || szBuffer <= 0)
        return WRONG_ARGUMENTS;
    if (szBuffer > BLOCK_SIZE) {
        return MEMORY_LACK;
    }
    VA blockAddr = findBlockAddr(ptr);
    return readFromBlock(blockAddr, pBuffer, szBuffer);
}
