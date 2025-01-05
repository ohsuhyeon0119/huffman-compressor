#ifndef BYTETABLE_H
#define BYTETABLE_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 

typedef struct ByteInfo ByteInfo;
typedef struct ByteTable ByteTable;

struct ByteInfo{
    uint64_t count;    
    uint8_t* codeword; 
} ;


struct ByteTable{
    ByteInfo* table[256];
};


ByteTable* ByteTable_create();
void ByteTable_increment(ByteTable* bt, uint8_t byte);
void ByteTable_set_codeword(ByteTable* bt, uint8_t byte, const uint8_t* codeword);
void ByteTable_print(ByteTable* bt);
void ByteTable_destroy(ByteTable* bt);


uint8_t* ByteTable_make_codewords_map_metadata(ByteTable* bt, size_t* metadata_size);

#endif 
