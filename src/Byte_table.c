#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Byte_table.h"
#include <stdint.h>
#include <math.h> 


ByteTable* ByteTable_create() {
    ByteTable* bt = (ByteTable*)malloc(sizeof(ByteTable));
    if (!bt) {
        perror("Failed to allocate ByteTable");
        return NULL;
    }

    
    for (int i = 0; i < 256; i++) {
        bt->table[i] = NULL;
    }
    return bt;
}


void ByteTable_increment(ByteTable* bt, uint8_t byte) {
    if (!bt->table[byte]) {
        
        bt->table[byte] = (ByteInfo*)malloc(sizeof(ByteInfo));
        bt->table[byte]->count = 0;
        bt->table[byte]->codeword = NULL;
    }
    bt->table[byte]->count++;
}


void ByteTable_set_codeword(ByteTable* bt, uint8_t byte, const uint8_t* codeword) {
    if (bt->table[byte]) {
        if (bt->table[byte]->codeword) {
            free(bt->table[byte]->codeword); 
        }
        bt->table[byte]->codeword = (uint8_t*) strdup((const char*)codeword); 
    }
}


void ByteTable_print(ByteTable* bt) {
    printf("\n--- Byte Table ---\n");
    for (int i = 0; i < 256; i++) {
        if (bt->table[i]) {
            printf("Byte '%c' (%d): Count = %lu, Codeword = %s\n",
                   (i >= 32 && i <= 126) ? i : '.', 
                   i, (unsigned long) bt->table[i]->count, 
                   bt->table[i]->codeword ? (char*)(bt->table[i]->codeword) : "(null)");
        }
    }
}


void ByteTable_destroy(ByteTable* bt) {
    for (int i = 0; i < 256; i++) {
        if (bt->table[i]) {
            free(bt->table[i]->codeword); 
            free(bt->table[i]);          
        }
    }
    free(bt);
}



uint8_t* ByteTable_make_codewords_map_metadata(ByteTable* bt, size_t* metadata_size) {
    if (!bt || !metadata_size) {
        fprintf(stderr, "Invalid arguments to ByteTable_make_codewords_map_metadata\n");
        return NULL;
    }

    
    size_t total_size = 0;
    for (int i = 0; i < 256; i++) {
        if (bt->table[i] && bt->table[i]->codeword) {
            size_t codeword_length = strlen((char*)bt->table[i]->codeword); 
            total_size += 1;  
            total_size += 1;  
            total_size += (codeword_length + 7) / 8; 
        }
    }

    
    uint8_t* metadata = (uint8_t*)malloc(total_size);
    if (!metadata) {
        perror("Failed to allocate memory for codewords_map_metadata");
        exit(EXIT_FAILURE);
    }

    
    size_t offset = 0;
    for (int i = 0; i < 256; i++) {
        if (bt->table[i] && bt->table[i]->codeword) {
            uint8_t target_character = (uint8_t)i; 
            size_t codeword_length = strlen((char*)bt->table[i]->codeword); 

            
            metadata[offset++] = target_character;

            
            metadata[offset++] = (uint8_t)codeword_length;

            
            uint8_t code_buffer = 0;
            int bit_offset = 0;
            for (size_t j = 0; j < codeword_length; j++) {
                if (bt->table[i]->codeword[j] == '1') {
                    code_buffer |= (1 << (7 - bit_offset)); 
                }
                bit_offset++;

                if (bit_offset == 8 || j == codeword_length - 1) { 
                    metadata[offset++] = code_buffer;
                    code_buffer = 0;
                    bit_offset = 0;
                }
            }
        }
    }

    
    *metadata_size = total_size;

    return metadata;
}