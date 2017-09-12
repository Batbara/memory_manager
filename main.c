#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include "mmemory.h"

#define MAX_POOL_SIZE 32768
#define ADDRESS_CAPACITY 16
#define BLOCK_SIZE 2
struct block {
    struct block* next;
    VA beginningAddress;
    char* data;
    int blockSize;
};

void push(struct block ** head, VA address, int blockSize) {
    struct block * current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(struct block));
    current->beginningAddress=address;
    current->data = NULL;
    current->blockSize=blockSize;
    current->next->next = NULL;
}

int main() {
    printf("Hello, World!\n");

    _init(1,5);
    return 0;
}
VA convertToVA(int blockNumber){
    //VA *address = (VA*)calloc(ADDRESS_CAPACITY,sizeof(char));
    VA address = calloc (16,sizeof (char) * ADDRESS_CAPACITY); /* n is the length of the array */
    int i;

    for (i=0; i<ADDRESS_CAPACITY; i++)
    {
        address[i] = '0';
    }
    for (int bit = strlen(address); bit>=0; bit--){
        if(blockNumber==0)
            break;
        int remainder = blockNumber%2;
        blockNumber/=2;
        char bitToAdd = (char)(remainder+'0');
       // printf("%c",bitToAdd);
        address[bit]= bitToAdd;
    }

    /*for (int bit = 0; bit<strlen(address); bit++){
        printf("%c",address[bit]);
    }
    printf("\n");*/
    return address;
}
int _init (int n, int szPage){
    struct block * pool;

    pool = malloc(sizeof(struct block));
    pool->next=NULL;
    pool->data=NULL;
    pool->beginningAddress=convertToVA(0);

    for ( int unit =1; unit<MAX_POOL_SIZE; unit++){
        VA address = convertToVA(unit);
        push(&pool,address,BLOCK_SIZE);
    }
    return 0;
}